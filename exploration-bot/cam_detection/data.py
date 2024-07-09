import os
import numpy as np
import face_recognition
  	  	
def list_files(folder_path):
    files_list = []
    for root, _, files in os.walk(folder_path):
        for file_name in files:
            file_path = os.path.join(root, file_name)
            files_list.append(file_path)
    return files_list



def encoding(image_path_list):
	encoding_list = []
	for image_path in image_path_list:
		image = face_recognition.load_image_file(image_path)
		face_encoding = face_recognition.face_encodings(image)[0]
		encoding_list.append(face_encoding)
	return encoding_list


base_path = os.getcwd()
known_face_dir = base_path + "/known"
known_face_path = list_files(known_face_dir)
known_face_encodings = encoding(known_face_path)

def main() :
	print(known_face_path)
	print(known_face_encodings)
		
if __name__ == "__main__":
	main()
