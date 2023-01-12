#ifndef UCONFIG_H
#define UCONFIG_H

// 定义 INI 配置文件结构体
typedef struct {
    char *key;   // 配置项的键
    char *value; // 配置项的值
    } IniConfig;

// 定义 INI 配置文件的管理器结构体
typedef struct {
    char *filename;   // INI 文件的文件名
    IniConfig *items; // INI 文件的配置项数组
    int num_items;    // INI 文件的配置项数量
    } IniConfigManager;

IniConfigManager *ini_config_manager_create(const char *filename);      // 创建 INI 配置文件管理器
void ini_config_manager_free(IniConfigManager *manager);    // 释放 INI 配置文件管理器
int ini_config_manager_read(IniConfigManager *manager);     // 读取 INI 配置文件
const char *ini_config_manager_get(IniConfigManager *manager, const char *key);     // 获取 INI 配置文件中的某个配置项的值
int ini_config_manager_save(IniConfigManager *manager);     // 保存 INI 配置文件
int ini_config_manager_remove(IniConfigManager *manager, const char *key);      // 从 INI 配置文件中删除某个配置项
int ini_config_manager_set(IniConfigManager *manager, const char *key, const char *value);      // 修改或者新增某个配置项

#endif // ULOG_H
