import base64
import requests
import sys
import json
import codecs


token = sys.argv[1]
port = sys.argv[2]
url = sys.argv[3]
# 操作类型 0 get 1 post 2 delete
type = sys.argv[4]

# 定义获取 LCU 端点的函数
def get_lcu_connection_info():
    # 连接到本地客户端（一般情况下默认是localhost:4999）
    return {'host': '127.0.0.1', 'port': port}

# 获取 LCUs 密钥
def get_auth_token():
    # 这里需要替换为你的 LCU 的用户认证信息
    username = 'riot'
    password = token
    return (username, password)

# 定义一个函数用来发送请求
def send_request_get(url):
    response = requests.get(f"https://{connection_info['host']}:{connection_info['port']}/{url}",
                            auth=auth_token,
                            verify=False)  # 避免 SSL 验证
    return response.json()  # 返回 JSON 格式的数据

def read_json_file(file_path):
    with codecs.open(file_path, 'r','utf-8') as file:
        data = json.load(file)
        return data
def send_request_post(url):
    json_data = read_json_file("./perk.txt")
    print(json_data)

    response = requests.post(f"https://{connection_info['host']}:{connection_info['port']}/{url}",
                            json=json_data,
                            auth=auth_token,
                            verify=False)  # 避免 SSL 验证
    return response.json()  # 返回 JSON 格式的数据

def send_request_delete(url):
    response = requests.delete(f"https://{connection_info['host']}:{connection_info['port']}/{url}",
                            auth=auth_token,
                            verify=False)  # 避免 SSL 验证
    return response.json()  # 返回 JSON 格式的数据

def send_request_patch(url):
    json_data = read_json_file("./champ.txt")
    print(json_data)
    response = requests.patch(f"https://{connection_info['host']}:{connection_info['port']}/{url}",
                            json=json_data,
                            auth=auth_token,
                            verify=False)  # 避免 SSL 验证
    return response.json()  # 返回 JSON 格式的数据

if __name__ == '__main__':
    # 打印获取的IP和端口
    connection_info = get_lcu_connection_info()
    auth_token = get_auth_token()
    # print(f"连接到 LCU 的地址是：{connection_info['host']}:{connection_info['port']},token:{auth_token}")
    # print(type)
    requests.packages.urllib3.disable_warnings()
    if type == '0':
        game_data = send_request_get(url)
        print(game_data)
    elif type == '1':
        game_data = send_request_post(url)
        print(game_data)
    elif type == '2':
        game_data = send_request_delete(url)
        print(game_data)
    elif type == '3':
        game_data = send_request_patch(url)
        print(game_data)
    
    
    





