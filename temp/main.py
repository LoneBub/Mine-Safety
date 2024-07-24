from urllib.request import urlopen
import cv2
import numpy as np

url='http://192.168.0.106/cam-lo.jpg'

while True:
    imgResp=urlopen(url)
    imgNp=np.array(bytearray(imgResp.read()),dtype=np.uint8)
    img=cv2.imdecode(imgNp,-1)

    flipped_frame = cv2.flip(img, 0)

    # all the opencv processing is done here
    cv2.imshow('test',flipped_frame)
    if ord('q')==cv2.waitKey(10):
        exit(0)
