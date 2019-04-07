import os
import numpy as np

from keras.models import Sequential
from keras.layers import Activation, Lambda
from keras import backend as K

from tsumaki.image_utils import resize

from tsumaki.model_zoo.deeplabv3plus.model import Deeplabv3

data_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")



class Model:
    def __init__(self, neural_dim):
        self.neural_dim = neural_dim
        self.nn_predictor = Sequential(
            layers=[
                Deeplabv3(input_shape=(neural_dim, neural_dim, 3), classes=1, backbone="mobilenetv2", weights=None),
                Activation("sigmoid"),
            ]
        )
        self.nn_predictor.load_weights(os.path.join(data_path, "./256_dice_coef_vocaug.h5"))

    def predict_multi(self, images):
        if len(images) == 0:
            return []
        resized_images, padded_images = [], []
        for image in images:
            resized_image, padded_image = self._crop(image)
            resized_images.append(resized_image)
            padded_images.append(padded_image)
        
        scaled = self._scale(np.stack(padded_images))
        preds = self.nn_predictor.predict(scaled)[:, :, :, 0]
        masks = (preds * 255).astype(np.uint8)

        results = []
        for mask, ri, img in zip(masks, resized_images, images):
            upscaled_mask = resize(mask[:ri.shape[0], :ri.shape[1]], img.shape[:2][::-1])
            results.append(upscaled_mask)
        return results

    def predict(self, image):
        return self.predict_multi([image])[0]

    def _crop(self, img, use_nearest=False):
        dim = self.neural_dim
        h, w = img.shape[:2]
        ratio = dim / max(h, w)
        new_h, new_w = int(h * ratio), int(w * ratio)
        resized = resize(img, (new_w, new_h))
        pad_h, pad_w = dim - resized.shape[0], dim - resized.shape[1]
        if len(img.shape) == 2:
            return resized, np.pad(resized, ((0,pad_h),(0,pad_w)),mode='constant')
        else:
            return resized, np.pad(resized, ((0,pad_h),(0,pad_w),(0,0)),mode='constant')

    def _scale(self, img):
        return (img / 127.5 - 1).astype(np.float32)


