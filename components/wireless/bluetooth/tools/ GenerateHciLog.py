import sys
import os
import re
import time
import datetime

output_log = r"\hci.log"
file_name = sys.argv[1]


writ_head_flag = 0
frist_write = 0
def gen_snoop_data(input):
   
    head                    = [0x62,0x74,0x73,0x6e,0x6f,0x6f,0x70,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x03,0xea]
    snoop_head              = []
    snoop_original          = [0x00,0x00,0x00]
    snoop_capture           = [0x00,0x00,0x00]
    snoop_flag              = [0x00,0x00,0x00]
    snoop_dropped_packets   = [0x00,0x00,0x00,0x00]
    snoop_timestamp         = []
    snoop_type              = []
    snoop_hcipacket         = []
    snoop                   = []
    hci_raw                 = []
    global writ_head_flag 
    global frist_write
    try:
        #Command pkt
        if int(input['type'],16) == 0x2:
            
            slen = len(input['data'])
            data_l = int(input['data'][0:2],16)
            data_h = int(input['data'][2:4],16)
            opcode = data_h << 8 | data_l

            hci_len = int(input['data'][4:6],16)
            
            snoop_type.append(0x01)
            snoop_flag.append(0x02)
            
            snoop_original.append(hci_len + 4)
            snoop_capture.append(hci_len + 4)
            
            snoop_hcipacket.append(data_l)
            snoop_hcipacket.append(data_h)
            snoop_hcipacket.append(hci_len)
            
         
            #print(input['data'][6:slen])
            
            s = input['data'][6:slen]
            for bit in range(0,hci_len*2,2):
                hci_raw_data = (int(s[bit:bit+2], 16))
                hci_raw.append(hci_raw_data)
            
            #print("hci_raw",hci_raw)
            t0 = time.time()
            t_ms=int(round(t0 * 1000))
            t_hex = hex(t_ms) 

            for i in range(0,8):
                t=((t_ms >> 8*i) & 255)             
                snoop_timestamp.append(t)    
                
            if snoop_timestamp:
                snoop_timestamp.reverse()

            snoop = snoop_original + snoop_capture + snoop_flag + snoop_dropped_packets + snoop_timestamp + snoop_type + snoop_hcipacket + hci_raw
            #print("snoop",snoop)
            
         
        #Event pkt       
        elif int(input['type'],16) == 0x3:
            
            slen = len(input['data'])
            hci_len = int(input['data'][2:4],16)
            snoop_type.append(0x04)
            snoop_flag.append(0x03)
            
            snoop_original.append(hci_len + 3)
            snoop_capture.append(hci_len + 3)
            
            type = int(input['data'][0:2],16)
            #print("type",type)
            
            snoop_hcipacket.append(type)
            snoop_hcipacket.append(hci_len)
            
            
            s = input['data'][4:slen]
           
            #print("s",s)
            for bit in range(0,hci_len*2,2):
                hci_raw_data = (int(s[bit:bit+2], 16))
                hci_raw.append(hci_raw_data)
            
            #print("hci_raw",hci_raw)

            t0 = time.time()
            t_ms=int(round(t0 * 1000))
            t_hex = hex(t_ms) 

            for i in range(0,8):
                t=((t_ms >> 8*i) & 255)             
                snoop_timestamp.append(t)    
                
            if snoop_timestamp:
                snoop_timestamp.reverse()

            snoop = snoop_original + snoop_capture + snoop_flag + snoop_dropped_packets + snoop_timestamp + snoop_type + snoop_hcipacket + hci_raw 

            #print("Event snoop",snoop)
          
            
        #ACL TX pkt       
        elif int(input['type'],16) == 0x4:  
            
            slen = len(input['data'])
            snoop_type.append(0x02)
            snoop_flag.append(0x00)
            len_l = int(input['data'][4:6],16)
            len_h = int(input['data'][6:8],16)
            
            hci_len = len_h << 8 | len_l
            #print("hci_len",hci_len)
            snoop_original.append(hci_len + 5)
            snoop_capture.append(hci_len + 5)
            
            handle = int(input['data'][0:2],16)
            snoop_hcipacket.append(handle)
            
            pb = int(input['data'][2:3],16)
            snoop_hcipacket.append(pb)
            
            snoop_hcipacket.append(len_l)
            snoop_hcipacket.append(len_h)
            
            s = input['data'][8:slen]
            #print("s",s)
            for bit in range(0,hci_len*2,2):
                hci_raw_data = (int(s[bit:bit+2], 16))
                hci_raw.append(hci_raw_data)
            
            #print("hci_raw",hci_raw)
            
            t0 = time.time()
            t_ms=int(round(t0 * 1000))
            t_hex = hex(t_ms) 
            
            for i in range(0,8):
                t=((t_ms >> 8*i) & 255)             
                snoop_timestamp.append(t)    
                
            if snoop_timestamp:
                snoop_timestamp.reverse()

            snoop = snoop_original + snoop_capture + snoop_flag + snoop_dropped_packets + snoop_timestamp + snoop_type + snoop_hcipacket + hci_raw
            #print("ACL TX pkt",snoop_hcipacket)
            
          
        #ACL RX pkt    
        elif int(input['type'],16) == 0x5: 
            
            slen = len(input['data'])
            snoop_type.append(0x02)
            snoop_flag.append(0x01)
            len_l = int(input['data'][4:6],16)
            len_h = int(input['data'][6:8],16)

            hci_len = len_h << 8 | len_l
            #print("hci_len",hci_len)
            snoop_original.append(hci_len + 5)
            snoop_capture.append(hci_len + 5)
             
            handle = int(input['data'][0:2],16)
            snoop_hcipacket.append(handle)
            
            pb = int(input['data'][2:4],16)
            snoop_hcipacket.append(pb)
            
            snoop_hcipacket.append(len_l)
            snoop_hcipacket.append(len_h)
            
            s = input['data'][8:slen]
            #print("s",s)
            for bit in range(0,hci_len*2,2):
                hci_raw_data = (int(s[bit:bit+2], 16))
                hci_raw.append(hci_raw_data)
                
            #print("hci_raw",hci_raw)
            
            t0 = time.time()
            t_ms=int(round(t0 * 1000))
            t_hex = hex(t_ms) 
            
            for i in range(0,8):
                t=((t_ms >> 8*i) & 255)             
                snoop_timestamp.append(t)    
                
            if snoop_timestamp:
                snoop_timestamp.reverse()

            snoop = snoop_original + snoop_capture + snoop_flag + snoop_dropped_packets + snoop_timestamp + snoop_type + snoop_hcipacket + hci_raw    
            #print("ACL RX pkt",snoop_hcipacket)

        else:
            print("input",input)
            raise

        if snoop:
            if frist_write == 0:
            
                global out_file_path
                out_file_path = path + output_log
                if os.path.exists(out_file_path):
                    os.remove(out_file_path)
                else:
                    print("Output file is not exist")
                
                frist_write = 1
                snoop_head = head
                snoop = snoop_head + snoop
            try:
                fbt=open(out_file_path,"ab+")
                fbt.write(bytearray(snoop))
                fbt.close()
            except IOError:
                print ("IOerror")
            
    except :  
        print("input",input)
        raise 
        
def str_handle(in_str,result):
   
    hci_regex = re.compile(r"\[Hci\]:pkt_type:\[(.+?)\],pkt_data:\[(.+?)\]")
    hci_data = hci_regex.findall(in_str)
    
    if hci_data:
        result['type']= hci_data[0][0]
        result['data']= hci_data[0][1]    
        gen_snoop_data(result)
        
    
def hcilog2bin(input_file):
    hci_data = {}

    with open(input_file,mode = "r",encoding="gbk",errors='ignore') as f:
        lines = f.readlines() 
        for line in lines:
            str_handle(line,hci_data)                

def search_dir_log(sname):
    global input_file_path
    dir = os.path.dirname(os.path.realpath(__file__))
    filelist = os.listdir(dir)
   
    for filename in filelist:
        de_path = os.path.join(dir, filename)
        if os.path.isfile(de_path):
            if de_path.endswith(sname): 
                input_file_path = de_path   
                
        
if __name__=='__main__':
    print("Input log:",file_name)
    try:
        search_dir_log(file_name)
        path =os.path.dirname(os.path.realpath(__file__))
   
        if os.path.exists(file_name) == True :
            hcilog2bin(file_name)  
        if os.path.exists(out_file_path) == True :
            print("Output log :",out_file_path)
    except Exception as e:
        print(e.args)
