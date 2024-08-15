import socket
import time

def connect_to_server(ip, port):
    # 创建一个socket对象
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # 连接到服务器
        client_socket.connect((ip, port))
        
        # 发送一条消息到服务器
        while True:
            message = b'Hello, Server!'
            client_socket.sendall(message)
            
            # 接收服务器的响应
            response = client_socket.recv(1024)
            print("Received:", response.decode())

            time.sleep(1)
        
    except ConnectionRefusedError:
        print(f"Connection refused by the server at {ip}:{port}")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        # 关闭socket
        client_socket.close()

if __name__ == "__main__":
    connect_to_server('192.168.1.234', 44818)