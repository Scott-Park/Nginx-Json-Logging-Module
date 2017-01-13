 /* 
 * Copyright (c) 2012 Ju-ho Park <vispourtoimeme@gmail.com>
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <jansson.h>

static ngx_str_t   str_request_body = ngx_string("request_body");
static ngx_str_t   ngx_http_json_log;

typedef struct {
  ngx_str_t       name;
} ngx_http_jsonlog_param_t;

typedef struct {
  //ngx_open_file_t *logpath; // if you want to create logfile by nginx, use this.(you must implement code in ngx_http_jsonlog_set_log)
  ngx_array_t     *secure;
  ngx_uint_t      disable; // deleted conf and no use
} ngx_http_jsonlog_loc_conf_t;

/* define module functions */
static ngx_int_t ngx_http_jsonlog_handler(ngx_http_request_t *r);
static void *ngx_http_jsonlog_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_jsonlog_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static char *ngx_http_jsonlog_set_secure(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_http_jsonlog_set_log(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_jsonlog_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_get_variable_value(ngx_http_request_t *r, ngx_str_t hval, u_char *req_name, json_t *j_obj);
static ngx_int_t ngx_http_jsonlog_write_log(u_char *buf, size_t len);

/* static variables for logging fields */
static ngx_str_t logging_fields[] = {
  ngx_string("remote_addr"),
  ngx_string("request"),
  ngx_string("request_method"),
  ngx_string("status"),
  ngx_string("time_local"),
  ngx_string("uri"),
};


/* module configuration commands */
static ngx_command_t ngx_http_jsonlog_commands[] = {
  {
    ngx_string("j_log"),
    NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_TAKE1,
    ngx_http_jsonlog_set_log,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL
  },
  {
    ngx_string("j_secure"),
    NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_ANY,
    ngx_http_jsonlog_set_secure,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL
  },

  ngx_null_command
};

/* module context */
static ngx_http_module_t ngx_http_jsonlog_module_ctx = {
  NULL,
  ngx_http_jsonlog_init,                 /* postconfiguration */
  NULL,
  NULL,
  NULL,
  NULL,
  ngx_http_jsonlog_create_loc_conf,      /* create location configuration */
  ngx_http_jsonlog_merge_loc_conf        /* merge location configuration */
};

ngx_module_t ngx_http_jsonlog_module = {
  NGX_MODULE_V1,
  &ngx_http_jsonlog_module_ctx,          /* module context */
  ngx_http_jsonlog_commands,             /* module directives */
  NGX_HTTP_MODULE,                        /* module type */
  NULL,                                  /* init master */
  NULL,                                  /* init module */
  NULL,                                  /* init process */
  NULL,                                  /* init thread */
  NULL,                                  /* exit thread */
  NULL,                                  /* exit process */
  NULL,                                  /* exit master */
  NGX_MODULE_V1_PADDING
};

static ngx_int_t
ngx_http_get_variable_value(ngx_http_request_t *r, ngx_str_t hval, u_char *req_name, json_t *j_obj)
{
  u_char                        *low, *uc_fval;
  ngx_http_variable_value_t     *ngx_hval;

  low = ngx_pnalloc(r->pool, hval.len);
  if(low == NULL){
    return NGX_ERROR;
  }
  ngx_uint_t key = ngx_hash_strlow(low, hval.data, hval.len);
  ngx_hval = ngx_http_get_variable(r, &hval, key);

  uc_fval = ngx_pnalloc(r->pool, ngx_hval->len+1);
  if(uc_fval == NULL){
    return NGX_ERROR;
  }
  ngx_cpystrn(uc_fval, ngx_hval->data, ngx_hval->len+1);
  json_object_set_new(j_obj, (char *)req_name, json_string((const char *)uc_fval));

  return NGX_OK;
}

/* core handler */
static ngx_int_t
ngx_http_jsonlog_handler(ngx_http_request_t *r)
{
  ngx_http_jsonlog_loc_conf_t  *jllcf;
  ngx_http_jsonlog_param_t     *jlparam;
  ngx_list_part_t               *part;
  ngx_table_elt_t               *header;
  ngx_http_variable_value_t     *ngx_hval;
  ngx_uint_t                    j, h;
  u_char                        *uc_line, *uc_fval, *p, *q;
  size_t                        len = 0;

  json_t *j_obj = json_object();
  jllcf = ngx_http_get_module_loc_conf(r, ngx_http_jsonlog_module);

  jlparam = jllcf->secure->elts;

  part = &r->headers_in.headers.part;
  header = part->elts;

  for (j = 0;  ; j++) {
    if (j >= part->nelts) {
      if (part->next == NULL) {
        break;
      }
      part = part->next;
      header = part->elts;
      j = 0;
    }
    json_object_set_new(j_obj, (char *)header[j].key.data, json_string((const char *)header[j].value.data));
  }

  ngx_http_get_variable_value(r, logging_fields[0], (u_char *)"Remote-addr", j_obj);
  ngx_http_get_variable_value(r, logging_fields[1], (u_char *)"Request", j_obj);
  ngx_http_get_variable_value(r, logging_fields[2], (u_char *)"Method", j_obj);
  ngx_http_get_variable_value(r, logging_fields[3], (u_char *)"Status", j_obj);
  ngx_http_get_variable_value(r, logging_fields[4], (u_char *)"Request-date", j_obj);
  ngx_http_get_variable_value(r, logging_fields[5], (u_char *)"Uri", j_obj);

  /* Imitate the rewrite module. */
  if(ngx_strncmp(r->main->method_name.data, (unsigned char *)"POST", 4) == 0){
    u_char *low;
    low = ngx_pnalloc(r->pool, str_request_body.len);
    if(low == NULL){
      return NGX_OK;
    }
    ngx_uint_t key = ngx_hash_strlow(low, str_request_body.data, str_request_body.len);
    ngx_hval = ngx_http_get_variable(r, &str_request_body, key);

    for(h = 0; h < jllcf->secure->nelts; h++){
      q = (u_char *)ngx_strstr(ngx_hval->data, jlparam[h].name.data);
      if(q){
        q += jlparam[h].name.len + 1;
        while(q < ngx_hval->data + ngx_hval->len){
          if(*q == (u_char)'&'){
            break;
          }
          *q = (u_char)'*';
          q++;
        }
      }
    }

    uc_fval = ngx_pnalloc(r->pool, ngx_hval->len+1);
    if(uc_fval == NULL){
      return NGX_OK;
    }
    ngx_cpystrn(uc_fval, ngx_hval->data, ngx_hval->len+1);
    json_object_set_new(j_obj, "Post-body", json_string((const char *)uc_fval));
  }

  json_object_set_new(j_obj, "Server", json_string((const char *)"Nginx"));
  char *json_str = json_dumps(j_obj, JSON_COMPACT);
  len = strlen(json_str);
  uc_line = ngx_pnalloc(r->pool, len+1);
  p = uc_line;
  p = ngx_cpymem(p, json_str, len);
  ngx_linefeed(p); // "\x0a"

  ngx_http_jsonlog_write_log(uc_line, len+1);

  json_decref(j_obj);

  return NGX_OK;
}

static ngx_int_t
ngx_http_jsonlog_write_log(u_char *buf, size_t len)
{
  ngx_tm_t                 tm;
  ngx_fd_t                 log_fd;
  
  /*
  * I recommend you use logrotate.
  * This code was used for testing.
  */
  u_char                   fileName[ngx_http_json_log.len+11];

  ngx_gmtime(ngx_time() + ngx_cached_time->gmtoff * 60, &tm);
  ngx_sprintf(fileName, "%s-%04d-%02d-%02d", ngx_http_json_log.data, tm.ngx_tm_year, tm.ngx_tm_mon, tm.ngx_tm_mday);

  log_fd = ngx_open_file(fileName, NGX_FILE_APPEND, NGX_FILE_CREATE_OR_OPEN, NGX_FILE_DEFAULT_ACCESS);
  (void)ngx_write_fd(log_fd, buf, len);
  ngx_close_file(log_fd);

  return NGX_OK;
}

static void *
ngx_http_jsonlog_create_loc_conf(ngx_conf_t *cf)
{
  ngx_http_jsonlog_loc_conf_t *jllcf;

  jllcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_jsonlog_loc_conf_t));
  if(jllcf == NULL){
    return NULL;
  }

  return jllcf;
}

static char *
ngx_http_jsonlog_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
  ngx_http_jsonlog_loc_conf_t *prev = parent;
  ngx_http_jsonlog_loc_conf_t *jllcf = child;
  ngx_http_jsonlog_param_t *jlparam;

  if(jllcf->secure){
    return NGX_CONF_OK;
  }

  jllcf->secure = prev->secure;
  //jllcf->disable = prev->disable;

  /* double check */
  if(jllcf->secure){
    return NGX_CONF_OK;
  }

  jllcf->secure = ngx_array_create(cf->pool, 1, sizeof(ngx_http_jsonlog_param_t));
  if(jllcf->secure == NULL){
    return NGX_CONF_ERROR;
  }

  jlparam = ngx_array_push(jllcf->secure);
  if(jlparam == NULL){
    return NGX_CONF_ERROR;
  }

  return NGX_CONF_OK;
}

static char *
ngx_http_jsonlog_set_secure(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
  ngx_http_jsonlog_loc_conf_t  *jllcf;
  ngx_http_jsonlog_param_t     *param;
  ngx_str_t                     *sec_val;
  ngx_uint_t                    i;

  jllcf = conf;
  sec_val = cf->args->elts;

  if (cf->args->elts == NULL){
    return NGX_CONF_OK;
  }

  if(jllcf->secure == NULL){
    jllcf->secure = ngx_array_create(cf->pool, cf->args->nelts, sizeof(ngx_http_jsonlog_param_t));
    if(jllcf->secure == NULL){
      return NGX_CONF_ERROR;
    }
  }

  for(i = 1; i <  cf->args->nelts; i++){
    param = ngx_array_push(jllcf->secure);
    if(param == NULL){
      return NGX_CONF_ERROR;
    }

    param->name = sec_val[i];
  }

  return NGX_CONF_OK;

}


static char *
ngx_http_jsonlog_set_log(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
  ngx_str_t                     *s_val;

  s_val = cf->args->elts;
  ngx_http_json_log = s_val[1];

  return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_jsonlog_init(ngx_conf_t *cf)
{
  ngx_http_core_main_conf_t     *cmcf;
  ngx_http_handler_pt           *handler;

  cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

  handler = ngx_array_push(&cmcf->phases[NGX_HTTP_LOG_PHASE].handlers);
  if(handler == NULL){
    return NGX_ERROR;
  }

  *handler = ngx_http_jsonlog_handler;

  return NGX_OK;
}
