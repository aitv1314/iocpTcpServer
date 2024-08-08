import socket
import time
# 创建一个UDP套接字
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
interface_ip = '192.168.1.234'
sock.bind((interface_ip, 0))
# 设置套接字选项，允许广播
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

# 广播地址通常为255.255.255.255，但具体取决于你的网络配置
broadcast_address = "255.255.255.255"
port = 44818

# 要发送的数据
message = b"c\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"


try:
    # 发送广播数据
    while True:
        sock.sendto(message, (broadcast_address, port))
        time.sleep(2)
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    # 关闭套接字
    sock.close()