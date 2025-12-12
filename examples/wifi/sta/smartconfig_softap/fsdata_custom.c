#include "lwip/apps/fs.h"
#include "lwip/def.h"
#include <string.h>
#include "cJSON.h"

/* HTML content - headers added by lwIP (LWIP_HTTPD_DYNAMIC_HEADERS=1) */
static const char data_index_html[] = 
"<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
"<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
"<title>BL602 WiFi</title><style>"
"*{box-sizing:border-box}"
"body{font-family:-apple-system,Arial,sans-serif;max-width:400px;margin:0 auto;padding:20px;background:#f0f2f5}"
"h1{color:#1a73e8;text-align:center;margin-bottom:20px}"
".card{background:#fff;padding:20px;border-radius:12px;box-shadow:0 2px 8px rgba(0,0,0,0.1)}"
"button{background:#1a73e8;color:#fff;border:none;padding:12px;border-radius:8px;width:100%;font-size:16px;cursor:pointer}"
"button:disabled{background:#ccc}"
".btn-cancel{background:#666;margin-top:8px}"
"input{width:100%;padding:12px;margin:10px 0;border:1px solid #ddd;border-radius:8px;font-size:16px}"
"input:focus{outline:none;border-color:#1a73e8}"
".list{list-style:none;padding:0;margin:15px 0;max-height:300px;overflow-y:auto}"
".item{display:flex;align-items:center;background:#f8f9fa;padding:12px;margin:8px 0;border-radius:8px;cursor:pointer;border:2px solid transparent}"
".item:hover,.item.sel{background:#e8f0fe;border-color:#1a73e8}"
".ssid{font-weight:500;flex:1}"
".signal{display:flex;align-items:flex-end;gap:2px;height:16px}"
".bar{width:4px;background:#1a73e8;border-radius:1px}"
".bar.off{background:#ddd}"
".msg{margin-top:15px;padding:12px;border-radius:8px;text-align:center}"
".ok{background:#d4edda;color:#155724}"
".err{background:#f8d7da;color:#721c24}"
".info{background:#cce5ff;color:#004085}"
".hide{display:none}"
"#form h3{margin:0 0 10px;color:#333}"
"</style></head><body>"
"<h1>WiFi Setup</h1>"
"<div class=\"card\">"
"<button id=\"scanBtn\" onclick=\"scan()\">Scan Networks</button>"
"<ul id=\"list\" class=\"list\"></ul>"
"<div id=\"form\" class=\"hide\">"
"<h3 id=\"ssidName\"></h3>"
"<input type=\"password\" id=\"pwd\" placeholder=\"Enter password\">"
"<button id=\"connBtn\" onclick=\"conn()\">Connect</button>"
"<button class=\"btn-cancel\" onclick=\"cancel()\">Cancel</button>"
"</div>"
"<div id=\"msg\" class=\"hide\"></div>"
"</div>"
"<script>"
"var ssid='',pt=null,ef=0;"
"function sig(r){var b='',l=r>-50?4:r>-60?3:r>-70?2:1;"
"for(var i=1;i<=4;i++)b+='<div class=\"bar'+(i<=l?'':' off')+'\" style=\"height:'+(i*4)+'px\"></div>';return b;}"
"function scan(){"
"var b=document.getElementById('scanBtn');"
"b.disabled=true;b.textContent='Scanning...';"
"document.getElementById('list').innerHTML='';"
"document.getElementById('msg').className='hide';"
"document.getElementById('form').className='hide';"
"fetch('/wifiscan').then(r=>r.text()).then(t=>{"
"b.disabled=false;b.textContent='Scan Networks';"
"try{var d=JSON.parse(t.trim())}catch(e){show('Parse error','err');return}"
"var l=document.getElementById('list');"
"if(!d||!d.length||d[0]==='null'){show('No networks found','err');return}"
"d.forEach(x=>{if(x.ssid){"
"var li=document.createElement('li');li.className='item';"
"li.innerHTML='<span class=\"ssid\">'+x.ssid+'</span><div class=\"signal\">'+sig(x.rssi)+'</div>';"
"li.onclick=function(){sel(this,x.ssid)};l.appendChild(li);}});"
"}).catch(()=>{b.disabled=false;b.textContent='Scan Networks';show('Scan failed','err');});}"
"function sel(el,s){"
"document.querySelectorAll('.item').forEach(i=>i.classList.remove('sel'));"
"el.classList.add('sel');ssid=s;"
"document.getElementById('ssidName').textContent=s;"
"document.getElementById('form').className='';document.getElementById('pwd').value='';document.getElementById('pwd').focus();}"
"function conn(){"
"var p=document.getElementById('pwd').value,b=document.getElementById('connBtn');"
"b.disabled=true;b.textContent='Connecting...';show('Connecting to '+ssid+'...','info');"
"fetch('/wificonfig?ssid='+encodeURIComponent(ssid)+'&password='+encodeURIComponent(p))"
".then(r=>r.text()).then(t=>{"
"try{var d=JSON.parse(t.trim())}catch(e){show('Error','err');b.disabled=false;b.textContent='Connect';return}"
"if(d.state==='connecting'){poll(b);}else{show('Failed','err');b.disabled=false;b.textContent='Connect';}"
"}).catch(()=>{b.disabled=false;b.textContent='Connect';show('Request failed','err');});}"
"function poll(b){"
"var c=0;ef=0;if(pt)clearInterval(pt);"
"pt=setInterval(()=>{"
"if(++c>90){clearInterval(pt);show('Timeout','err');b.disabled=false;b.textContent='Connect';return;}"
"show('Connecting... '+c+'s','info');"
"fetch('/wifiget').then(r=>r.text()).then(t=>{"
"ef=0;try{var d=JSON.parse(t.trim())}catch(e){return}"
"if(d.state==='connected'||d.state==='ap_closed'){clearInterval(pt);"
"var m='Connected!<br>IP: '+d.ip;if(d.state==='ap_closed')m+='<br>AP Closed';else if(d.msg)m+='<br>'+d.msg;"
"show(m,'ok',1);document.getElementById('form').className='hide';b.disabled=false;b.textContent='Connect';}"
"}).catch(()=>{if(++ef>10){clearInterval(pt);show('Lost','err');b.disabled=false;b.textContent='Connect';}});},1000);}"
"function cancel(){if(pt)clearInterval(pt);document.getElementById('form').className='hide';"
"document.getElementById('connBtn').disabled=false;document.getElementById('connBtn').textContent='Connect';}"
"function show(m,c,h){var e=document.getElementById('msg');h?e.innerHTML=m:e.textContent=m;e.className='msg '+c;}"
"</script></body></html>";

/* FS_ROOT must be defined for lwIP fs.c, set to NULL */
#define FS_ROOT NULL

void *fs_state_init(struct fs_file *file, const char *name) { return NULL; }
void fs_state_free(struct fs_file *file, void *state) {}

int fs_open_custom(struct fs_file *file, const char *name)
{
    /* Serve index.html for root */
    if (name[0] == '/' && (name[1] == '\0' || 
        strcmp(name, "/index.html") == 0 || strcmp(name, "/index.htm") == 0)) {
        memset(file, 0, sizeof(struct fs_file));
        file->data = data_index_html;
        file->len = sizeof(data_index_html) - 1;
        file->index = file->len;
        return 1;
    }
    
    /* Handle JSON response from CGI */
    if (name[0] == '[' || name[0] == '{') {
        cJSON *obj = cJSON_Parse(name);
        if (obj) {
            cJSON_Delete(obj);
            memset(file, 0, sizeof(struct fs_file));
            file->data = name;
            file->len = strlen(name);
            file->index = file->len;
            return 1;
        }
    }
    
    return 0;
}

void fs_close_custom(struct fs_file *file)
{
    if (file) {
        file->data = NULL;
        file->pextension = NULL;
    }
}

int fs_read_custom(struct fs_file *file, char *buffer, int count)
{
    if (!file || !file->data) return -1;
    int remain = file->len - file->index;
    if (remain <= 0) return -1;
    int n = (count < remain) ? count : remain;
    memcpy(buffer, file->data + file->index, n);
    file->index += n;
    return n;
}
