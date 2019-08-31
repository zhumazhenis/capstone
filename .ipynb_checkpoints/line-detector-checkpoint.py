import cv2
roadBGRImg = cv2.imread('road.jpg')
roadGrayImg = cv2.cvtColor(roadBGRImg, cv2.COLOR_BGR2GRAY)
cv2.imshow('gray', roadGrayImg)
cv2.waitKey(0)