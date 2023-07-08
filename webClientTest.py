
import requests
import time
import json
from csv import DictWriter
import os
import datetime
#get 요청
def get_request():

    URL = 'http://192.168.0.177/'
    response = requests.get(URL)
    data = response.json()


    data['time']=datetime.datetime.now()
    #csv 파일에 추가
    print(data)
    with open("data.csv", 'a', newline='') as f_object:
        dictwriter_object = DictWriter(f_object, fieldnames=data.keys())
        if os.path.getsize('data.csv') == 0:
            dictwriter_object.writeheader()
        dictwriter_object.writerow(data)
        f_object.close()




while True:
    get_request()
    time.sleep(1)
