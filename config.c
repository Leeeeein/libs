#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
// 创建 INI 配置文件管理器
IniConfigManager *ini_config_manager_create(const char *filename)
    {
    // 为管理器分配内存
    IniConfigManager *manager = malloc(sizeof(IniConfigManager));
    if (manager == NULL) {
        return NULL;
    }

    // 为文件名分配内存并复制
    manager->filename = malloc(strlen(filename) + 1);
    if (manager->filename == NULL) {
        free(manager);
        return NULL;
    }
    strcpy(manager->filename, filename);

    // 初始化配置项数组和数量
    manager->items = NULL;
    manager->num_items = 0;

    return manager;
    }

// 释放 INI 配置文件管理器
void ini_config_manager_free(IniConfigManager *manager) {
    // 释放文件名和配置项数组
    free(manager->filename);
    free(manager->items);

    // 释放管理器本身
    free(manager);
}

// 读取 INI 配置文件
int ini_config_manager_read(IniConfigManager *manager) {
    // 打开文件
    FILE *fp = fopen(manager->filename, "r");
    if (fp == NULL) {
    return -1;
    }

    // 初始化配置项数组和数量
    manager->items = NULL;
    manager->num_items = 0;

    // 读取文件的每一行
    char line[1024];
    while (fgets(line, sizeof(line), fp) != NULL) {
    // 去除行末的换行符
    int len = strlen(line);
    if (line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    // 如果行是注释或空行，则跳过
    if (line[0] == '#' || line[0] == '\0') {
        continue;
    }

    // 分离键和值
    char *equals = strchr(line, '=');
    if (equals == NULL) {
        continue;
    }
    *equals = '\0';
    char *key = line;
    char *value = equals + 1;

    // 添加到配置项数组中
    manager->items = realloc(manager->items, sizeof(IniConfig) * (manager->num_items + 1));
    if (manager->items == NULL) {
        fclose(fp);
        return -1;
    }
    IniConfig config;
    config.key = strdup(key);
    config.value = strdup(value);
    manager->items[manager->num_items] = config;
    manager->num_items++;
    }

    // 关闭文件
    fclose(fp);
    return 0;
}

// 获取 INI 配置文件中的某个配置项的值
const char *ini_config_manager_get(IniConfigManager *manager, const char *key) {
  for (int i = 0; i < manager->num_items; i++) {
    if (strcmp(manager->items[i].key, key) == 0) {
      return manager->items[i].value;
    }
  }
  return NULL;
}

// 保存 INI 配置文件
int ini_config_manager_save(IniConfigManager *manager) {
  // 打开文件
  FILE *fp = fopen(manager->filename, "w");
  if (fp == NULL) {
    return -1;
  }

  // 写入配置项
  for (int i = 0; i < manager->num_items; i++) {
    fprintf(fp, "%s=%s\n", manager->items[i].key, manager->items[i].value);
  }

  // 关闭文件
  fclose(fp);
  return 0;
}

// 修改或者新增某个配置项
int ini_config_manager_set(IniConfigManager *manager, const char *key, const char *value) {
    // 查找配置项
    int index = -1;
    for (int i = 0; i < manager->num_items; i++) {
    if (strcmp(manager->items[i].key, key) == 0) {
        index = i;
        break;
    }
    }

    // 如果找到了，则修改值
    if (index != -1) {
    free(manager->items[index].value);
    manager->items[index].value = strdup(value);
    return 0;
    }

    // 如果没找到，则添加新配置项
    manager->items = realloc(manager->items, sizeof(IniConfig) * (manager->num_items + 1));
    if (manager->items == NULL) {
    return -1;
    }
    IniConfig config;
    config.key = strdup(key);
    config.value = strdup(value);
    manager->items[manager->num_items] = config;
    manager->num_items++;
    return 0;
}

// 从 INI 配置文件中删除某个配置项
int ini_config_manager_remove(IniConfigManager *manager, const char *key) {
    // 查找配置项
    int index = -1;
    for (int i = 0; i < manager->num_items; i++) {
    if (strcmp(manager->items[i].key, key) == 0) {
        index = i;
        break;
    }
    }

    // 如果找到了，则从数组中删除
    if (index != -1) {
    free(manager->items[index].key);
    free(manager->items[index].value);
    for (int i = index; i < manager->num_items - 1; i++) {
        manager->items[i] = manager->items[i + 1];
    }
    manager->num_items--;
    return 0;
    }

    // 如果没找到，则返回错误
    return -1;
}

// int main(int argc, char *argv[]) {
//     // 创建 INI 配置文件管理器
//     IniConfigManager *manager = ini_config_manager_create("config.ini");
//     if (manager == NULL) {
//         printf("Error creating INI config manager.\n");
//     return 1;
//     }

//     // 读取 INI 配置文件
//     if (ini_config_manager_read(manager) != 0) {
//         printf("Error reading INI config file.\n");
//     ini_config_manager_free(manager);
//     return 1;
//     }

//     // 遍历并打印配置项
//     // for (int i = 0; i < manager->num_items; i++) {
//     //     printf("%s = %s\n", manager->items[i].key, manager->items[i].value);
//     // }
//     char* ssss = ini_config_manager_get(manager, "items");
//     if(ssss == NULL)
//         printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

//     // 释放 INI 配置文件管理器
//     ini_config_manager_free(manager);
//     return 0;
// }
