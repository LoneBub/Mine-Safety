import cv2
import numpy as np
import requests

base_url = 'http://192.168.0.106:81'

response = requests.get(base_url+"/stream", stream=True)
buffer = bytes()

for chunk in response.iter_content(chunk_size=3072):
    buffer += chunk
    a = buffer.find(b'\xff\xd8')
    b = buffer.find(b'\xff\xd9')

    if a != -1 and b != -1:
        jpg = buffer[a:b+2]
        buffer = buffer[b+2:]

        # Check if jpg is not empty
        if jpg:
            # Convert the byte array to a numpy array
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            
            # Check if frame is not None (imdecode fails)
            if frame is not None:
                frame = cv2.flip(frame, 0)
                
                # Display the image
                cv2.imshow('IP Camera Stream', frame)

                key = cv2.waitKey(1)
                if key == ord('q'):
                    exit(0)
            else:
                print("Failed to decode frame.")
        else:
            print("Received empty image data.")

cv2.destroyAllWindows()

