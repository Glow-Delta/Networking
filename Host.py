from flask import Flask, request

app = Flask(__name__)

@app.route('/data', methods=['POST'])
def receive_data():
    value = request.form.get('value')
    
    if value:
        print(f"Received value: {value}")
        return f"Data received: {value}", 200 
    else:
        return "No value received", 400

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80)