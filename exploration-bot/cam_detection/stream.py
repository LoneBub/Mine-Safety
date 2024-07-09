import cv2
import requests
import numpy as np

# Load pre-trained face detection model
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

# Assuming you have defined base_address somewhere
base_address = "http://192.168.189.209:81"

response = requests.get(base_address + "/stream", stream=True)
buffer = bytes()

for i, chunk in enumerate(response.iter_content(chunk_size=1024)):
    buffer += chunk
    a = buffer.find(b'\xff\xd8')
    b = buffer.find(b'\xff\xd9')

    if a != -1 and b != -1:
        jpg = buffer[a:b + 2]
        buffer = buffer[b + 2:]

        if jpg:
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)

            if frame is not None:
                frame = cv2.flip(frame, 0)
                
                # Convert frame to grayscale for face detection
                gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

                # Detect faces
                faces = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

                # Draw rectangle and label around each detected face
                for (x, y, w, h) in faces:
                    cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                    cv2.putText(frame, 'Face', (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

                #desired_width = 800
                #desired_height = 600
                resized_frame = frame#cv2.resize(frame, (desired_width, desired_height))

                cv2.imshow('IP Camera Stream', resized_frame)
                key = cv2.waitKey(1)
                if key == ord('q'):
                    exit(0)
            else:
                print("Failed to decode frame.")
        else:
            print("Received empty image data.")

cv2.destroyAllWindows()

