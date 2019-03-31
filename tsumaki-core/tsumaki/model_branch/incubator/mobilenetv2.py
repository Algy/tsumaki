import os
import numpy as np

from keras.models import Sequential
from keras.layers import Activation, Lambda
from keras import backend as K

from scipy.misc import imresize

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

    def predict(self, image):
        h, w = image.shape[:2]
        resized_image, padded_image = self._crop(image)
        pred = self.nn_predictor.predict(self._scale(padded_image[np.newaxis, :, :, :]))[0, :, :, 0]
        pred = pred[:resized_image.shape[0], :resized_image.shape[1]]
        mask = (pred * 255).astype(np.uint8)
        upscaled_mask = imresize(mask, (h, w))
        return upscaled_mask 

    def _crop(self, img, use_nearest=False):
        dim = self.neural_dim
        h, w = img.shape[:2]
        ratio = dim / max(h, w)
        new_h, new_w = int(h * ratio), int(w * ratio)
        resized = imresize(img, (new_h, new_w))
        pad_h, pad_w = dim - resized.shape[0], dim - resized.shape[1]
        if len(img.shape) == 2:
            return resized, np.pad(resized, ((0,pad_h),(0,pad_w)),mode='constant')
        else:
            return resized, np.pad(resized, ((0,pad_h),(0,pad_w),(0,0)),mode='constant')

    def _scale(self, img):
        return (img / 127.5 - 1).astype(np.float32)


