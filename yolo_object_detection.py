import cv2
import numpy as np
import argparse

#Adauga calea catre fisier ca argument la momentul apelului scriptului python
parser = argparse.ArgumentParser()

parser.add_argument('-f', '--file', type=str, required=True, help="Defineste calea catre fisier")
parser.add_argument('-z', '--zoom', type=int, required=True, help="Defineste valoarea zoom-ului aplicat imaginii")

args = parser.parse_args()

image_path = args.file
image_zoom = args.zoom

# Incarcam fisierele generate de Yolo3 care contin training-ul pentru placutele de inmatriculare
net = cv2.dnn.readNet("yolov3_training_last.weights", "yolov3_testing.cfg")

# Name custom object
classes = ["plate"]

layer_names = net.getLayerNames()
output_layers = [layer_names[i[0] - 1] for i in net.getUnconnectedOutLayers()]


# Loading image
img = cv2.imread(str(image_path))
height, width, channels = img.shape


# Detecting objects
blob = cv2.dnn.blobFromImage(img, 0.00392, (416, 416), (0, 0, 0), True, crop=False)

net.setInput(blob)
outs = net.forward(output_layers)


#Getting x,y coordonates and width and height
for out in outs:
    for detection in out:
        scores = detection[5:]
        class_id = np.argmax(scores)
        confidence = scores[class_id]
        if confidence > 0.3:
            # Object detected
            center_x = int(detection[0] * width)
            center_y = int(detection[1] * height)
            w = int(detection[2] * width)
            h = int(detection[3] * height)

            # Rectangle coordinates
            x = int(center_x - w / 2)
            y = int(center_y - h / 2)

#Original plate
plateImage = img[y:y+h, x:x+w]

#Zoom value
zoom = image_zoom

#Zoomed in plate
zoomedPlateImage = cv2.resize(plateImage, None, fx=zoom, fy=zoom)

cv2.imwrite("plate.jpg", zoomedPlateImage)
key = cv2.waitKey(0)

cv2.destroyAllWindows()