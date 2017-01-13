# Nginx Json logging module
This module logging reqeust data(include post-body) to json format.
<br>And support dynamic module

Basically nginx is support access.log and can modify log format to json.
<br>I just want to understand how nginx work.
<br>This module is not perfect because it was written for learning purposes.

This module tested on Nginx 1.10.2(stable) version.

## Output data in log file>

```
{
"Status":"200",
"Origin":"http://192.168.10.131",
"Host":"192.168.10.131",
"Connection":"keep-alive",
"Content-Length":"104",
"Cache-Control":"max-age=0",
"Accept-Language":"ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4",
"Upgrade-Insecure-Requests":"1",
"Accept":"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
"User-Agent":"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36",
"Content-Type":"application/x-www-form-urlencoded",
"Referer":"http://192.168.10.131/wp-login.php?loggedout=true",
"Method":"POST",
"Accept-Encoding":"gzip, deflate",
"Request-date":"13/Jan/2017:17:30:56 +0900","Cookie":"wp-settings-time-1=1484282864; wordpress_test_cookie=WP+Cookie+check",
"Remote-addr":"192.168.10.1",
"Request":"POST /wp-login.php HTTP/1.1",
"Uri":"/wp-login.php",
"Post-body":"log=soctt&pwd=******&wp-submit=Log+In&redirect_to=http%3A%2F%2F192.168.10.131%2Fwp-admin%2F&testcookie=1",
"Server":"Nginx"
}

```

# Installation>
## Dependency
For using this module, you should be require jansson library(i use 2.7.1 version)<br>

**[Ubuntu]**

```
# sudo apt-get update
# sudo apt-get install libjansson-dev
```

**[CentOS]**

```
# yum -y install epel-release
# yum -y install jansson-devel
```

## configuration(for dynamic module)
The following configuration information is used when installing the package.
<br>Additional options are added for dynamic module use.
* added:  `--add-dynamic-module=/root/ngx-json-module --with-ld-opt=-ljansson`

**[Ubuntu 16.04 xenial]**

```
./configure --prefix=/etc/nginx --sbin-path=/usr/sbin/nginx --modules-path=/usr/lib/nginx/modules --conf-path=/etc/nginx/nginx.conf --error-log-path=/var/log/nginx/error.log --http-log-path=/var/log/nginx/access.log --pid-path=/var/run/nginx.pid --lock-path=/var/run/nginx.lock --http-client-body-temp-path=/var/cache/nginx/client_temp --http-proxy-temp-path=/var/cache/nginx/proxy_temp --http-fastcgi-temp-path=/var/cache/nginx/fastcgi_temp --http-uwsgi-temp-path=/var/cache/nginx/uwsgi_temp --http-scgi-temp-path=/var/cache/nginx/scgi_temp --user=nginx --group=nginx --with-file-aio --with-threads --with-ipv6 --with-http_addition_module --with-http_auth_request_module --with-http_dav_module --with-http_flv_module --with-http_gunzip_module --with-http_gzip_static_module --with-http_mp4_module --with-http_random_index_module --with-http_realip_module --with-http_secure_link_module --with-http_slice_module --with-http_ssl_module --with-http_stub_status_module --with-http_sub_module --with-http_v2_module --with-mail --with-mail_ssl_module --with-stream --with-stream_ssl_module --with-cc-opt='-g -O2 -fstack-protector-strong -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2' --with-ld-opt='-Wl,-Bsymbolic-functions -Wl,-z,relro -Wl,-z,now -Wl,--as-needed' --add-dynamic-module=/root/ngx-json-module --with-ld-opt=-ljansson
```

**[Ubuntu 14.04 truty]**

```
./configure --prefix=/etc/nginx --sbin-path=/usr/sbin/nginx --modules-path=/usr/lib/nginx/modules --conf-path=/etc/nginx/nginx.conf --error-log-path=/var/log/nginx/error.log --http-log-path=/var/log/nginx/access.log --pid-path=/var/run/nginx.pid --lock-path=/var/run/nginx.lock --http-client-body-temp-path=/var/cache/nginx/client_temp --http-proxy-temp-path=/var/cache/nginx/proxy_temp --http-fastcgi-temp-path=/var/cache/nginx/fastcgi_temp --http-uwsgi-temp-path=/var/cache/nginx/uwsgi_temp --http-scgi-temp-path=/var/cache/nginx/scgi_temp --user=nginx --group=nginx --with-file-aio --with-threads --with-ipv6 --with-http_addition_module --with-http_auth_request_module --with-http_dav_module --with-http_flv_module --with-http_gunzip_module --with-http_gzip_static_module --with-http_mp4_module --with-http_random_index_module --with-http_realip_module --with-http_secure_link_module --with-http_slice_module --with-http_ssl_module --with-http_stub_status_module --with-http_sub_module --with-http_v2_module --with-mail --with-mail_ssl_module --with-stream --with-stream_ssl_module --with-cc-opt='-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2' --with-ld-opt='-Wl,-Bsymbolic-functions -Wl,-z,relro -Wl,-z,now -Wl,--as-needed' --add-dynamic-module=/root/ngx-json-module --with-ld-opt=-ljansson
```

**[CentOS 7]**

```
./configure --prefix=/etc/nginx --sbin-path=/usr/sbin/nginx --modules-path=/usr/lib64/nginx/modules --conf-path=/etc/nginx/nginx.conf --error-log-path=/var/log/nginx/error.log --http-log-path=/var/log/nginx/access.log --pid-path=/var/run/nginx.pid --lock-path=/var/run/nginx.lock --http-client-body-temp-path=/var/cache/nginx/client_temp --http-proxy-temp-path=/var/cache/nginx/proxy_temp --http-fastcgi-temp-path=/var/cache/nginx/fastcgi_temp --http-uwsgi-temp-path=/var/cache/nginx/uwsgi_temp --http-scgi-temp-path=/var/cache/nginx/scgi_temp --user=nginx --group=nginx --with-file-aio --with-threads --with-ipv6 --with-http_addition_module --with-http_auth_request_module --with-http_dav_module --with-http_flv_module --with-http_gunzip_module --with-http_gzip_static_module --with-http_mp4_module --with-http_random_index_module --with-http_realip_module --with-http_secure_link_module --with-http_slice_module --with-http_ssl_module --with-http_stub_status_module --with-http_sub_module --with-http_v2_module --with-mail --with-mail_ssl_module --with-stream --with-stream_ssl_module --with-cc-opt='-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic' --add-dynamic-module=/root/ngx-json-module --with-ld-opt=-ljansson
```

**[CentOS 6]**

```
./configure --prefix=/etc/nginx --sbin-path=/usr/sbin/nginx --modules-path=/usr/lib64/nginx/modules --conf-path=/etc/nginx/nginx.conf --error-log-path=/var/log/nginx/error.log --http-log-path=/var/log/nginx/access.log --pid-path=/var/run/nginx.pid --lock-path=/var/run/nginx.lock --http-client-body-temp-path=/var/cache/nginx/client_temp --http-proxy-temp-path=/var/cache/nginx/proxy_temp --http-fastcgi-temp-path=/var/cache/nginx/fastcgi_temp --http-uwsgi-temp-path=/var/cache/nginx/uwsgi_temp --http-scgi-temp-path=/var/cache/nginx/scgi_temp --user=nginx --group=nginx --with-file-aio --with-threads --with-ipv6 --with-http_addition_module --with-http_auth_request_module --with-http_dav_module --with-http_flv_module --with-http_gunzip_module --with-http_gzip_static_module --with-http_mp4_module --with-http_random_index_module --with-http_realip_module --with-http_secure_link_module --with-http_slice_module --with-http_ssl_module --with-http_stub_status_module --with-http_sub_module --with-http_v2_module --with-mail --with-mail_ssl_module --with-stream --with-stream_ssl_module --with-cc-opt='-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -m64 -mtune=generic' --add-dynamic-module=/root/ngx-json-module --with-ld-opt=-ljansson
```

## Build

```
# make modules
```

## Apply module
1. Copy builded module to nginx module directroy.

  ```
  # cp objs/ngx_http_jsonlog_module.so /etc/nginx/modules/
  ```
  
2. Change nginx.conf to load jsonlog module

  ```
  # sed -i '1i load_module modules/ngx_http_jsonlog_module.so;' /etc/nginx/nginx.conf
  ```
  
3. Create jsonlog module config.
<br>**j_log**: log file path for logging.
<br>**j_secure**: convert sensitive data to asterisk in post-body(html post form data).

  ```
  # cat > /etc/nginx/conf.d/jsonlog.conf
  j_log /var/log/ngnix/jsonlog.log;
  j_secure pwd password;
  ```
  
4. Create log directroy and change permission.

  ```
  # mkdir /var/log/jsonlog
  # chown nginx:nginx /var/log/jsonlog
  ```
  
5. restart or reload service.

  ```
  # service nginx restart or reload
  ```
