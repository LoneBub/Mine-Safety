import json
def write_to_json(data):
    json_file = "server/data1.json"
    with open(json_file, 'w') as f:
        json.dump(data, f, indent=4)
    print(f"Data has been written to {json_file}")

def main():
    print("Enter data to be written to JSON file:")
    temp = int(input("temp: "))
    humidity = int(input("humidity: "))  # Assuming age is an integer
    angle = int(input("angle: "))

    data = {
        "Temp": temp,
        "Humidity": humidity,
        "angle": angle
    }

    write_to_json(data)

if __name__ == "__main__":
    main()