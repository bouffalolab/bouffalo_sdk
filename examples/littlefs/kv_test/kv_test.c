#include <stdint.h>
#include <stdbool.h>

#include "arch_os.h"
#include "arch_psm.h"
#include "arch_dbg.h"
#include "arch_crypto.h"

#define KV_OK    0    /* There is no error 						*/
#define KV_ERROR (-1) /* A generic error happens 					*/

arch_os_thread_handle_t kv_test_thread;
arch_os_thread_handle_t kv_test_thread2;
arch_os_thread_handle_t kv_test_thread3;
arch_os_thread_handle_t kv_test_thread5;

static arch_os_mutex_handle_t kv_test_store_mutex;
int kv_num = 0;
unsigned int kv_w_time = 0;
unsigned int kv_w_max_time = 0;
#define KV_TEST_MAX_VALID_NUM CONFIG_KV_TEST_FILE_OPER_SUM
unsigned int kv_test_valid_num = 0;

#undef KV_LOG_TAG
#define KV_LOG_TAG    "kv_test"

#define KV_MAX_KEY_LEN  64
#define KV_MAX_VAL_LEN  256

#define PSM_MAX_KEY_LEN KV_MAX_KEY_LEN
#define PSM_MAX_VAL_LEN KV_MAX_VAL_LEN

typedef struct st_kv_key {
    uint8_t valid;
    uint8_t keep;
    uint8_t type;
    int valuelen;
    char namespace[PSM_MAX_KEY_LEN / 2];
    char key[PSM_MAX_KEY_LEN / 2];
    char hash[16];
};

#define MAX_KEY_NUM 200
struct st_kv_key store_key[MAX_KEY_NUM];
#define KV_TEST_CHECK_NUM 60

#define KV_TEST_KEEP_KV   10
char kv_keep[KV_TEST_KEEP_KV + 1] = {
    1, 8, 13, 15, 25, 36, 42, 47, 50, /*52,56,*/ 64 /*,70,75,85,95, 110, 128,147,167*/
};                                                  //�±꣬��Щ����ɾ����ֻ���޸�

int kv_sector_erase_num[6] = { 0 };
static void kv_test_print_value(char *value, int len);
int arch_get_random(uint8_t *output, size_t output_len)
{
    unsigned int random = 0;
    int i, offset;

    for (i = 0, offset = 0; i < output_len; i++) {
        offset = i & 0x3;
        if (offset == 0)
            random = rand();
        output[i] = (random >> (offset * 8)) & 0xff;
    }

    return output_len;
}

static int kv_test_get_rand_num(void)
{
    int i;
    uint8_t valuestr[5];
    int value = 0;

    arch_get_random(valuestr, 2);
    value = valuestr[0] << 8 | valuestr[1];
    //LOG_INFO_TAG(KV_LOG_TAG, "rand num:%d", value);
    if (0 == value) {
        LOG_INFO_TAG(KV_LOG_TAG, "[%x][%x]", valuestr[0], valuestr[1]);
    }
    return value;
}

//ns_len=0
//key_len = 0
static void kv_test_create_key(char *namespace, int ns_len, char *key, int key_len)
{
    int i;
    int value = 0;
    int len;

    if (0 == ns_len) {
        value = kv_test_get_rand_num();
        len = value % (PSM_MAX_KEY_LEN / 2);
        if (0 == len % 2)
            len = value % 10;
        if (len == 0)
            len = 1;
    } else
        len = ns_len;

    //LOG_INFO_TAG(KV_LOG_TAG, "namespace len:%d", len);
    arch_get_random(namespace, len);
    for (i = 0; i < len; i++) {
        if (*(namespace + i) < 'a' || *(namespace + i) > 'z')
            *(namespace + i) = *(namespace + i) % 26 + 'a';
    }
    *(namespace + len) = 0;
    if (0 == key_len) {
        value = kv_test_get_rand_num();
        len = value % (PSM_MAX_KEY_LEN / 2);
        if (0 == len % 2)
            len = value % 10;
        if (len == 0)
            len = 1;
    } else
        len = key_len;
    //LOG_INFO_TAG(KV_LOG_TAG, "key len:%d", len);
    arch_get_random(key, len);
    for (i = 0; i < len; i++) {
        if (*(key + i) < 'a' || *(key + i) > 'z')
            *(key + i) = *(key + i) % 26 + 'a';
    }
    *(key + len) = 0;
}

static int kv_test_create_value_len(void)
{
    int value = 0;

    value = kv_test_get_rand_num();
    value %= (PSM_MAX_VAL_LEN);
    if ((value % 2) == 0)
        value %= 64;
    //if(0 == value)value = 16;
    //LOG_INFO_TAG(KV_LOG_TAG, " value len:%d", value);

    return value;
}

//len=0,�������
static int kv_test_create_value(char *value, int len)
{
    int valuelen = 0;
    int i;
    int ret;
    int flag = 0;

    if (0 == len)
        valuelen = kv_test_create_value_len();
    else
        valuelen = len;
    //if(valuelen < 2)
    //valuelen = 2;
    ret = arch_get_random(value, valuelen);
    if (ret != valuelen) {
        LOG_ERROR_TAG(KV_LOG_TAG, "rand ret != len");
    }
    for (i = 0; i < valuelen; i++) {
        if (0 == *(value + i)) {
            flag = 1;
            *(value + i) = 0x30;
        }
    }
    *(value + i) = 0;
    if (flag) {
        //printf("------------have zero-----len:%d--------\r\n", valuelen - 1);
        //kv_test_print_value(value, valuelen - 1);
    }
    return valuelen;
}

static int kv_test_find_invalid(void)
{
    int i;

    for (i = 0; i < MAX_KEY_NUM; i++) {
        if (0 == store_key[i].valid)
            break;
    }
    if (i >= MAX_KEY_NUM) {
        return KV_ERROR;
    }

    return i;
}

static int kv_test_find_valid(void)
{
    int i;
    int total = 0;
#if 0	
	
	int value = 0;
	int valid_num = 0;
#endif
    int max_long = 0;
    int m_l_index = -1;
    static int valid_check_flag = 0; //0ɾ���ģ�1ɾ��С��
#if 1                                //ɾ��valueֵ���
    if (1 == valid_check_flag) {
        max_long = KV_MAX_VAL_LEN;
    }
    for (i = 0; i < MAX_KEY_NUM; i++) {
        if (1 == store_key[i].valid && 0 == store_key[i].keep) {
            total++;
            if (0 == valid_check_flag) {
                if (store_key[i].valuelen > max_long) {
                    max_long = store_key[i].valuelen;
                    m_l_index = i;
                }
            } else {
                if (store_key[i].valuelen < max_long) {
                    max_long = store_key[i].valuelen;
                    m_l_index = i;
                }
            }
        }
    }
    valid_check_flag = !valid_check_flag;
    if (m_l_index >= 0) {
        LOG_INFO_TAG(KV_LOG_TAG, "will del[%d]->[%d]: %s.%s, valid:%d, keep:%d,len:%d", total, m_l_index,
                     store_key[m_l_index].namespace, store_key[m_l_index].key, store_key[m_l_index].valid,
                     store_key[m_l_index].keep, store_key[m_l_index].valuelen);
        return m_l_index;
    } else {
        LOG_ERROR_TAG(KV_LOG_TAG, "can not find valid kv");
        return KV_ERROR;
    }
#else //���ɾ��

    for (i = 0; i < MAX_KEY_NUM; i++) {
        if (1 == store_key[i].valid)
            total++;
    }

    if (0 == total) {
        LOG_ERROR_TAG(KV_LOG_TAG, "total kv:%d ", total);
        return KV_ERROR;
    }
    value = kv_test_get_rand_num();

    if (total > KV_TEST_KEEP_KV)
        value %= (total - KV_TEST_KEEP_KV);
    else
        value %= 10;
    value++; //�ڼ�������1��ʼ

    LOG_INFO_TAG(KV_LOG_TAG, "kv_test_find_valid total:%d, will del: %d", total, value);

    valid_num = 0;
    for (i = 0; i < MAX_KEY_NUM; i++) {
        if (1 == store_key[i].valid && 0 == store_key[i].keep) {
            valid_num++;
            if (valid_num == value)
                break;
        }
    }
    if (i >= MAX_KEY_NUM) {
        LOG_ERROR_TAG(KV_LOG_TAG, "can not find valid kv");
        return KV_ERROR;
    }

    LOG_INFO_TAG(KV_LOG_TAG, "will del: %s.%s, valid:%d, keep:%d", store_key[i].namespace, store_key[i].key,
                 store_key[i].valid, store_key[i].keep);
    return i;

#endif
}

static int kv_test_find_keep_valid(void)
{
    int i;

    int total = 0;
    int value = 0;
    int valid_num = 0;
    static int check_flag = 0; //0ɾ���ģ�1ɾ��С��

#if 1
    valid_num = -1;
    if (1 == check_flag) {
        value = KV_MAX_VAL_LEN;
    }
    for (i = 0; i < MAX_KEY_NUM; i++) {
        if (1 == store_key[i].valid && 1 == store_key[i].keep) {
            total++;
            //printf("[%d]value len:%d\r\n",i, store_key[i].valuelen);
            if (0 == check_flag) {
                if (store_key[i].valuelen > value) {
                    value = store_key[i].valuelen;
                    valid_num = i;
                }
            } else {
                if (store_key[i].valuelen < value) {
                    value = store_key[i].valuelen;
                    valid_num = i;
                }
            }
        }
    }
    check_flag = !check_flag;
    if (valid_num >= 0) {
        LOG_INFO_TAG(KV_LOG_TAG, "will modify[%d]->[%d]: %s.%s, valid:%d, keep:%d, len:%d", total, valid_num,
                     store_key[valid_num].namespace, store_key[valid_num].key, store_key[valid_num].valid,
                     store_key[valid_num].keep, store_key[valid_num].valuelen);
        return valid_num;
    } else {
        LOG_ERROR_TAG(KV_LOG_TAG, "can not find valid keep kv");
        return KV_ERROR;
    }
#else
    value = kv_test_get_rand_num();

    for (i = 0; i < MAX_KEY_NUM; i++) {
        if (1 == store_key[i].valid && 1 == store_key[i].keep)
            total++;
    }

    if (0 == total) {
        LOG_ERROR_TAG(KV_LOG_TAG, "total keep  valid kv:%d ", total);
        return KV_ERROR;
    }

    value %= total;

    value++; //�ڼ�������1��ʼ

    LOG_INFO_TAG(KV_LOG_TAG, "total keep:%d, will modify: %d", total, value);

    valid_num = 0;
    for (i = 0; i < MAX_KEY_NUM; i++) {
        if (1 == store_key[i].valid && 1 == store_key[i].keep) {
            valid_num++;
            if (valid_num == value)
                break;
        }
    }
    if (i >= MAX_KEY_NUM) {
        LOG_ERROR_TAG(KV_LOG_TAG, "can not find valid keep kv");
        return KV_ERROR;
    }
    LOG_INFO_TAG(KV_LOG_TAG, "will modify: %s.%s, valid:%d, keep:%d", store_key[i].namespace, store_key[i].key,
                 store_key[i].valid, store_key[i].keep);
    return i;
#endif
}

static void kv_test_save_key(char *namespace, char *key, char *value, int v_len, int type)
{
    int index = MAX_KEY_NUM;
    int i;

    for (i = 0; i < MAX_KEY_NUM; i++) {
        if (0 == strcmp(namespace, store_key[i].namespace) && 0 == strcmp(key, store_key[i].key)) {
            index = i;
            break;
        }
    }
    if (MAX_KEY_NUM == index) {
        index = kv_test_find_invalid();

        if (index < 0) {
            LOG_INFO_TAG(KV_LOG_TAG, "no invalid space: %d", index);
            return;
        }
    }

    memset(&store_key[index], 0, sizeof(struct st_kv_key));
    store_key[index].valid = 1;
    store_key[index].type = type;
    if (strlcpy(store_key[index].namespace, namespace, sizeof(store_key[index].namespace)) >= sizeof(store_key[index].namespace)) {
        printf("[OS]: strlcpy truncated \r\n"); 
    }
    if (strlcpy(store_key[index].key, key, sizeof(store_key[index].key)) >= sizeof(store_key[index].key)) {
        printf("[OS]: strlcpy truncated \r\n"); 
    }
    store_key[index].valuelen = v_len;
    arch_md5(value, v_len, store_key[index].hash);
    for (i = 0; i < KV_TEST_KEEP_KV; i++) {
        if (kv_keep[i] == index) {
            store_key[index].keep = 1;
        }
    }
}

static void kv_test_del_key(void)
{
    int index;
    int ret;

    arch_os_mutex_get(kv_test_store_mutex, ARCH_OS_WAIT_FOREVER);
    index = kv_test_find_valid();
    if (index < 0) {
        LOG_ERROR_TAG(KV_LOG_TAG, "#########del key find error:%d", index);
        arch_os_mutex_put(kv_test_store_mutex);
        return;
    }

    ret = arch_psm_erase_key(store_key[index].namespace, store_key[index].key);
    if (ret != KV_OK) {
        LOG_ERROR_TAG(KV_LOG_TAG, "#########erase key error: %d", ret);
        arch_os_mutex_put(kv_test_store_mutex);
        return;
    } else {
        char *value;
        LOG_ERROR_TAG(KV_LOG_TAG, "del key %s.%s success", store_key[index].namespace, store_key[index].key);
        value = malloc(KV_MAX_VAL_LEN);
        if (value == NULL) {
            LOG_ERROR_TAG(KV_LOG_TAG, "#########malloc error");
            memset(&store_key[index], 0, sizeof(struct st_kv_key));
            arch_os_mutex_put(kv_test_store_mutex);
            return;
        }
        if (store_key[index].type)
            ret = arch_psm_get_value(store_key[index].namespace, store_key[index].key, value, KV_MAX_VAL_LEN);
        else
            ret = arch_psm_get_str(store_key[index].namespace, store_key[index].key, value, KV_MAX_VAL_LEN);
        if (ret > 0) {
            LOG_ERROR_TAG(KV_LOG_TAG, "#########read ok after erase #############");
        }
        free(value);
    }
    memset(&store_key[index], 0, sizeof(struct st_kv_key));
    arch_os_mutex_put(kv_test_store_mutex);
}

static void kv_test_del_key1(void)
{
    int index;
    int ret;

    arch_os_mutex_get(kv_test_store_mutex, ARCH_OS_WAIT_FOREVER);

    for (int index = 0; index < MAX_KEY_NUM; index++) {
        if (store_key[index].valid != 1) {
            continue;
        }

        ret = arch_psm_erase_key(store_key[index].namespace, store_key[index].key);
        if (ret != KV_OK) {
            LOG_ERROR_TAG(KV_LOG_TAG, "##erase key %s.%s error: %d", store_key[index].namespace, store_key[index].key,
                          ret);
        }

        memset(&store_key[index], 0, sizeof(struct st_kv_key));
    }

    arch_os_mutex_put(kv_test_store_mutex);
    return;
}

static void kv_test_print_value(char *value, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (i % 32 == 0)
            printf("\r\n");
        printf("[%x]", *(value + i));
    }
    printf("\r\n");
}

static void kv_test_check_result(void)
{
    static int checknum = 1;
    int i;
    int ret;
    char *value;
    char hash[20];
    int error_no = 0;
    int total_valid = 0;
    int total_valid_v_len = 0;

    value = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == value)
        return;
    if (kv_num / KV_TEST_CHECK_NUM >= checknum) {
        arch_os_mutex_get(kv_test_store_mutex, ARCH_OS_WAIT_FOREVER);
        checknum++;

        for (i = 0; i < MAX_KEY_NUM; i++) {
            if (store_key[i].valid) {
                total_valid++;
                total_valid_v_len += store_key[i].valuelen;
                if (store_key[i].type)
                    ret = arch_psm_get_value(store_key[i].namespace, store_key[i].key, value, PSM_MAX_VAL_LEN);
                else
                    ret = arch_psm_get_str(store_key[i].namespace, store_key[i].key, value, PSM_MAX_VAL_LEN);
                if (ret >= 0) {
                    arch_md5(value, ret, hash);
                    if (memcmp(hash, store_key[i].hash, 16) != 0) {
                        error_no++;

                        LOG_ERROR_TAG(KV_LOG_TAG, "[%d] %s.%s hash error, write len: %d, read len: %d", i,
                                      store_key[i].namespace, store_key[i].key, store_key[i].valuelen, ret);
                        //kv_test_print_value(store_key[i].hash, 16);
                        //kv_test_print_value(value, ret);
                        //kv_test_print_value(hash, 16);
                    }
                } else {
                    error_no++;
                    LOG_ERROR_TAG(KV_LOG_TAG, "[%d] %s.%s read error %d", i, store_key[i].namespace, store_key[i].key,
                                  ret);
                }
            }
        }
        {
            LOG_INFO_TAG(KV_LOG_TAG, "-----------------------------------------");
            LOG_INFO_TAG(KV_LOG_TAG, "KV test total: %d", kv_num);
            LOG_INFO_TAG(KV_LOG_TAG, "KV valid num: %d", total_valid);
            LOG_INFO_TAG(KV_LOG_TAG, "KV valid len: %d", total_valid_v_len);
            LOG_INFO_TAG(KV_LOG_TAG, "KV Average write time: %d", kv_w_time / KV_TEST_CHECK_NUM);
            LOG_INFO_TAG(KV_LOG_TAG, "KV max write time: %d", kv_w_max_time);
            kv_w_time = 0;
            if (0 == error_no)
                LOG_INFO_TAG(KV_LOG_TAG, "^_^ OKAY");
            else
                LOG_INFO_TAG(KV_LOG_TAG, "FAIL");
            LOG_INFO_TAG(KV_LOG_TAG, "KV sector erase num:");
            for (i = 0; i < 6; i++) {
                printf("[%d]	", kv_sector_erase_num[i]);
            }
            printf("\r\n");
            LOG_INFO_TAG(KV_LOG_TAG, "=========================================");
        }
        arch_os_mutex_put(kv_test_store_mutex);
    }
    free(value);
}

static void kv_test_check_result1(void)
{
    static int checknum = 1;
    int i;
    int ret;
    char *value;
    char hash[20];
    int error_no = 0;
    int ok_no = 0;
    int total_valid = 0;
    int total_valid_v_len = 0;

    value = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == value)
        return;
    if (kv_num / KV_TEST_CHECK_NUM >= checknum) {
        arch_os_mutex_get(kv_test_store_mutex, ARCH_OS_WAIT_FOREVER);
        checknum++;

        for (i = 0; i < MAX_KEY_NUM; i++) {
            if (store_key[i].valid) {
                total_valid++;
                total_valid_v_len += store_key[i].valuelen;
                if (store_key[i].type)
                    ret = arch_psm_get_value(store_key[i].namespace, store_key[i].key, value, PSM_MAX_VAL_LEN);
                else
                    ret = arch_psm_get_str(store_key[i].namespace, store_key[i].key, value, PSM_MAX_VAL_LEN);

                if (ret >= 0) {
                    arch_md5(value, ret, hash);
                    if (memcmp(hash, store_key[i].hash, 16) != 0) {
                        error_no++;
                        LOG_ERROR_TAG(KV_LOG_TAG, "[%d] %s.%s hash error, write len: %d, read len: %d, error num: %d",
                                      i, store_key[i].namespace, store_key[i].key, store_key[i].valuelen, ret,
                                      error_no);
                    } else {
                        ok_no++;
                    }
                } else {
                    error_no++;
                    LOG_ERROR_TAG(KV_LOG_TAG, "[%d] %s.%s read error %d, error num: %d", i, store_key[i].namespace,
                                  store_key[i].key, ret, error_no);
                }
            }
        }
        {
            LOG_INFO_TAG(KV_LOG_TAG, "-----------------------------------------");
            LOG_INFO_TAG(KV_LOG_TAG, "KV test total: %d", kv_num);
            LOG_INFO_TAG(KV_LOG_TAG, "KV valid num: %d, ok num: %d, err no: %d", total_valid, ok_no, error_no);
            LOG_INFO_TAG(KV_LOG_TAG, "KV valid len: %d", total_valid_v_len);
            LOG_INFO_TAG(KV_LOG_TAG, "KV Average write time: %d", kv_w_time / KV_TEST_CHECK_NUM);
            LOG_INFO_TAG(KV_LOG_TAG, "KV max write time: %d", kv_w_max_time);
            kv_w_time = 0;
            if (0 == error_no)
                LOG_INFO_TAG(KV_LOG_TAG, "^_^ OKAY");
            else
                LOG_INFO_TAG(KV_LOG_TAG, "FAIL");
            LOG_INFO_TAG(KV_LOG_TAG, "KV sector erase num:");
            for (i = 0; i < 6; i++) {
                printf("[%d]	", kv_sector_erase_num[i]);
            }
            printf("\r\n");
            LOG_INFO_TAG(KV_LOG_TAG, "=========================================");
        }
        arch_os_mutex_put(kv_test_store_mutex);
        kv_test_del_key1();
    }

    free(value);
}

#if 0
void kv_test_check_flash_erase(uint32_t addr)
{
	uint32_t offset = 0;
	int sector_no;

	offset = addr - FW_PARAM_START_ADDR;

	sector_no = offset/4096;
	if(sector_no < 0 || sector_no > 6)
	{
		LOG_ERROR_TAG(KV_LOG_TAG, "erase addr error: %x", addr);
		return;
	}
	
	kv_sector_erase_num[sector_no] ++;
}
#endif

static void kv_test_proc(void *arg)
{
    int ret;
    bool type = 1;
    int valuelen = 0;
    int error_num = 0;
    char kv_namespace[PSM_MAX_KEY_LEN / 2];
    char kv_key[PSM_MAX_KEY_LEN / 2];
    char *kv_value, *kv_value2;
    int time;

    kv_value = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == kv_value)
        return;

    kv_value2 = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == kv_value2) {
        free(kv_value);
        return;
    }

    while (1) {
        arch_os_ms_sleep(CONFIG_KV_TEST_FILE_OPER_PERIOD);
        if (kv_test_valid_num > KV_TEST_MAX_VALID_NUM)
            continue;

        arch_os_mutex_get(kv_test_store_mutex, ARCH_OS_WAIT_FOREVER);

        kv_test_create_key(kv_namespace, 0, kv_key, 0);
        valuelen = kv_test_create_value(kv_value, 0);
        type = kv_test_get_rand_num() % 2;

        LOG_INFO_TAG(KV_LOG_TAG, "1 write kv %s.%s, value: %d", kv_namespace, kv_key, valuelen);
        time = arch_os_ms_now();

        if (type)
            ret = arch_psm_set_value(kv_namespace, kv_key, kv_value, valuelen);
        else
            ret = arch_psm_set_str(kv_namespace, kv_key, kv_value);

        if (ret == (type ? valuelen : valuelen + 1)) //write success
        {
            int time1 = arch_os_ms_elapsed(time);
            kv_w_time += time1;
            if (time1 > kv_w_max_time) {
                kv_w_max_time = time1;
            }
            kv_test_valid_num++;
            LOG_INFO_TAG(KV_LOG_TAG, "1 write kv time:%d", time1);

            kv_test_save_key(kv_namespace, kv_key, kv_value, ret, type);
            kv_num++;

            LOG_INFO_TAG(KV_LOG_TAG, "1 write kv success >>>> %d <<<<", kv_num);
            arch_os_mutex_put(kv_test_store_mutex);

            // kv_test_check_result();
            kv_test_check_result1();
        } else {
            error_num++;
            arch_os_mutex_put(kv_test_store_mutex);
            LOG_ERROR_TAG(KV_LOG_TAG, "#########1 write kv fail %d", ret);

            if (error_num > 2) {
                kv_test_del_key();
                arch_os_ms_sleep(10000);
                error_num = 0;
            }
        }
    }
}

static int kv_test_gen_namespace_len(void)
{
    static int ns_len = 0;

    ns_len++;
    if (ns_len >= PSM_MAX_KEY_LEN / 2)
        ns_len = 1;

    return ns_len;
}

static int kv_test_gen_key_len(void)
{
    static int k_len = 0;

    k_len++;
    if (k_len >= PSM_MAX_KEY_LEN / 2)
        k_len = 1;

    return k_len;
}

static int kv_test_gen_value_len(void)
{
    static int v_len = 0;
#if 1
    v_len++;
    if (v_len > PSM_MAX_VAL_LEN)
        v_len = 0;
#else
    v_len = KV_MAX_VAL_LEN;
#endif
    return v_len;
}

static void kv_test2_proc(void *arg)
{
    int ret;
    bool type = 1;
    int valuelen = 0;
    int error_num = 0;
    int namespace_l = 0;
    int key_l = 0;
    char kv_namespace[PSM_MAX_KEY_LEN / 2];
    char kv_key[PSM_MAX_KEY_LEN / 2];
    char *kv_value, *kv_value2;
    int time;

    kv_value = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == kv_value)
        return;

    kv_value2 = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == kv_value2) {
        free(kv_value);
        return;
    }

    char *value_test = malloc(PSM_MAX_VAL_LEN + 1);

    while (1) {
        arch_os_ms_sleep(CONFIG_KV_TEST_FILE_OPER_PERIOD);
        if (kv_test_valid_num > KV_TEST_MAX_VALID_NUM)
            continue;
        arch_os_mutex_get(kv_test_store_mutex, ARCH_OS_WAIT_FOREVER);
        namespace_l = kv_test_gen_namespace_len();
        key_l = kv_test_gen_key_len();
        kv_test_create_key(kv_namespace, namespace_l, kv_key, key_l);

        valuelen = kv_test_create_value(kv_value, kv_test_gen_value_len());
        type = kv_test_get_rand_num() % 2;

        LOG_INFO_TAG(KV_LOG_TAG, "2 write kv %s.%s, origin len: %d", kv_namespace, kv_key, valuelen);
        kv_test_print_value(kv_value, valuelen);

        time = arch_os_ms_now();
        if (type)
            ret = arch_psm_set_value(kv_namespace, kv_key, kv_value, valuelen);
        else
            ret = arch_psm_set_str(kv_namespace, kv_key, kv_value);
        if (ret == (type ? valuelen : valuelen + 1)) //write success
        {
            int time1 = arch_os_ms_elapsed(time);
            kv_w_time += time1;
            if (time1 > kv_w_max_time) {
                kv_w_max_time = time1;
            }
            kv_test_valid_num++;
            LOG_INFO_TAG(KV_LOG_TAG, "2 write kv time:%d", time1);
            kv_test_save_key(kv_namespace, kv_key, kv_value, ret, type);
            kv_num++;
            LOG_INFO_TAG(KV_LOG_TAG, "2 write kv success >>>%d<<<", kv_num);
            arch_os_mutex_put(kv_test_store_mutex);

#if 0
			memset(value_test, 0, PSM_MAX_VAL_LEN+1);
			if(type) {
				ret = arch_psm_get_value(kv_namespace, kv_key, value_test, PSM_MAX_VAL_LEN);
			} else {
				ret = arch_psm_get_str(kv_namespace, kv_key, value_test, PSM_MAX_VAL_LEN);
			}
			kv_test_print_value(value_test, ret);
#endif
            // kv_test_check_result();
            kv_test_check_result1();
        } else {
            error_num++;
            arch_os_mutex_put(kv_test_store_mutex);
            LOG_ERROR_TAG(KV_LOG_TAG, "#########2 write kv fail %d, num: %d", ret, error_num);
#if 1
            if (error_num > 2) {
                kv_test_del_key();
                arch_os_ms_sleep(10000);
                error_num = 0;
            }
#endif
        }
    }
}

static void kv_test3_proc(void *arg)
{
    int ret;
    bool type = 1;
    int valuelen = 0;
    int index;
    char kv_namespace[PSM_MAX_KEY_LEN / 2];
    char kv_key[PSM_MAX_KEY_LEN / 2];
    char *kv_value, *kv_value2;
    int time;

    kv_value = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == kv_value)
        return;

    kv_value2 = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == kv_value2) {
        free(kv_value);
        return;
    }

    char *value_test = malloc(PSM_MAX_VAL_LEN + 1);

    while (1) {
        arch_os_ms_sleep(1);
        index = kv_test_find_keep_valid();
        if (index < 0) {
            arch_os_ms_sleep(CONFIG_KV_TEST_FILE_OPER_PERIOD);
            continue;
        }
        arch_os_mutex_get(kv_test_store_mutex, ARCH_OS_WAIT_FOREVER);
        if (strlcpy(kv_namespace, store_key[index].namespace, sizeof(kv_namespace)) >= sizeof(kv_namespace)) {
            printf("[OS]: strlcpy truncated \r\n"); 
        }
        if (strlcpy(kv_key, store_key[index].key, sizeof(kv_key)) >= sizeof(kv_key)) {
            printf("[OS]: strlcpy truncated \r\n"); 
        }
        valuelen = kv_test_create_value(kv_value, 0);

        LOG_INFO_TAG(KV_LOG_TAG, "3 modify kv[%d] %s.%s, value:%d", index, store_key[index].namespace,
                     store_key[index].key, valuelen);
        type = store_key[index].type;
        time = arch_os_ms_now();
        if (type)
            ret = arch_psm_set_value(kv_namespace, kv_key, kv_value, valuelen);
        else
            ret = arch_psm_set_str(kv_namespace, kv_key, kv_value);
        if (ret == (type ? valuelen : valuelen + 1)) //write success
        {
            int time1;
            time1 = arch_os_ms_elapsed(time);
            kv_w_time += time1;
            if (time1 > kv_w_max_time) {
                kv_w_max_time = time1;
            }
            LOG_INFO_TAG(KV_LOG_TAG, "3 modify kv time:%d", time1);
            if (type)
                ret = arch_psm_get_value(kv_namespace, kv_key, kv_value2, PSM_MAX_VAL_LEN);
            else
                ret = arch_psm_get_str(kv_namespace, kv_key, kv_value2, PSM_MAX_VAL_LEN);
            if (0 == memcmp(kv_value, kv_value2, ret)) {
                //kv_test_save_key(kv_namespace, kv_key, kv_value, ret, type);
                arch_md5(kv_value, ret, store_key[index].hash);
                store_key[index].valuelen = ret;
                kv_num++;
                //kv_test_print_value(store_key[index].hash, 16);
                //kv_test_print_value(kv_value, 16);
                //kv_test_print_value(kv_value2, 16);
                LOG_INFO_TAG(KV_LOG_TAG, "3 modify kv[%d] %s.%s success >>>%d<<<", index, kv_namespace, kv_key,
                             kv_num);
            } else {
                LOG_ERROR_TAG(KV_LOG_TAG, "#########3 modify kv fail %d", ret);
                kv_test_print_value(kv_value, ret);
                kv_test_print_value(kv_value2, ret);
            }

            arch_os_mutex_put(kv_test_store_mutex);
            // kv_test_check_result();
            kv_test_check_result1();
        } else {
            arch_os_mutex_put(kv_test_store_mutex);
            LOG_ERROR_TAG(KV_LOG_TAG, "#########3 write kv fail %d will del", ret);

            if (type)
                ret = arch_psm_get_value(kv_namespace, kv_key, value_test, PSM_MAX_VAL_LEN);
            else
                ret = arch_psm_get_str(kv_namespace, kv_key, value_test, PSM_MAX_VAL_LEN);
            LOG_ERROR_TAG(KV_LOG_TAG, "write fail value name %s.%s: len: %d", kv_namespace, kv_key, ret);

            kv_test_del_key();
        }
    }
}

static void kv_test4_proc(void *arg)
{
    int ret;
    bool type = 1;
    int valuelen = 0;
    int error_num = 0;
    char kv_namespace[PSM_MAX_KEY_LEN / 2];
    char kv_key[PSM_MAX_KEY_LEN / 2];
    char *kv_value, *kv_value2;
    int time;

    kv_value = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == kv_value)
        return;

    kv_value2 = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == kv_value2) {
        free(kv_value);
        return;
    }

    while (1) {
        arch_os_ms_sleep(600);
        if (kv_test_valid_num > KV_TEST_MAX_VALID_NUM)
            continue;
        arch_os_mutex_get(kv_test_store_mutex, ARCH_OS_WAIT_FOREVER);
        kv_test_create_key(kv_namespace, 0, kv_key, 0);
        valuelen = kv_test_create_value(kv_value, 0);
        type = kv_test_get_rand_num() % 2;

        LOG_INFO_TAG(KV_LOG_TAG, "4 write kv %s.%s, value: %d", kv_namespace, kv_key, valuelen);
        time = arch_os_ms_now();
        if (type)
            ret = arch_psm_set_value(kv_namespace, kv_key, kv_value, valuelen);
        else
            ret = arch_psm_set_str(kv_namespace, kv_key, kv_value);
        if (ret == valuelen) //write success
        {
            int time1 = arch_os_ms_elapsed(time);
            kv_w_time += time1;
            if (time1 > kv_w_max_time) {
                kv_w_max_time = time1;
            }
            kv_test_valid_num++;
            LOG_INFO_TAG(KV_LOG_TAG, "4 write kv time:%d", time1);
            kv_test_save_key(kv_namespace, kv_key, kv_value, ret, type);
            kv_num++;
            LOG_INFO_TAG(KV_LOG_TAG, "4 write kv success >>>> %d <<<<", kv_num);
            arch_os_mutex_put(kv_test_store_mutex);
            kv_test_check_result1();
        } else {
            error_num++;
            arch_os_mutex_put(kv_test_store_mutex);
            LOG_ERROR_TAG(KV_LOG_TAG, "#########4 write kv fail %d", ret);
            if (error_num > 2) {
                kv_test_del_key();
                arch_os_ms_sleep(10000);
                error_num = 0;
            }
        }
    }
}

void kv_test5_proc(void *arg)
{
    int ret;
    bool type = 1;
    int valuelen = 0;
    int error_num = 0;
    char kv_namespace[PSM_MAX_KEY_LEN / 2];
    char kv_key[PSM_MAX_KEY_LEN / 2];
    char *kv_value, *kv_value2;
    int time;

    kv_value = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == kv_value)
        return;

    kv_value2 = malloc(PSM_MAX_VAL_LEN + 1);
    if (NULL == kv_value2) {
        free(kv_value);
        return;
    }

    while (1) {
        arch_os_ms_sleep(CONFIG_KV_TEST_FILE_OPER_PERIOD);
        if (kv_test_valid_num > KV_TEST_MAX_VALID_NUM)
            continue;
        arch_os_mutex_get(kv_test_store_mutex, ARCH_OS_WAIT_FOREVER);
        kv_test_create_key(kv_namespace, 0, kv_key, 0);
        valuelen = kv_test_create_value(kv_value, 0);
        type = 1;

        LOG_INFO_TAG(KV_LOG_TAG, "5 write kv %s.%s, value: %d", kv_namespace, kv_key, valuelen);
        time = arch_os_ms_now();
        if (type)
            ret = arch_psm_set_value(kv_namespace, kv_key, kv_value, valuelen);
        else
            ret = arch_psm_set_str(kv_namespace, kv_key, kv_value);
        if (ret == (type ? valuelen : valuelen + 1)) //write success
        {
            int time1 = arch_os_ms_elapsed(time);
            kv_w_time += time1;
            if (time1 > kv_w_max_time) {
                kv_w_max_time = time1;
            }
            kv_test_valid_num++;
            LOG_INFO_TAG(KV_LOG_TAG, "5 write kv time:%d", time1);
            kv_test_save_key(kv_namespace, kv_key, kv_value, ret, type);
            kv_num++;
            LOG_INFO_TAG(KV_LOG_TAG, "5 write kv success >>>> %d <<<<", kv_num);
            arch_os_mutex_put(kv_test_store_mutex);
            // kv_test_check_result();
            kv_test_check_result1();
        } else {
            error_num++;
            arch_os_mutex_put(kv_test_store_mutex);
            LOG_ERROR_TAG(KV_LOG_TAG, "#########5 write kv fail %d", ret);
            if (error_num > 2) {
                kv_test_del_key();
                arch_os_ms_sleep(10000);
                error_num = 0;
            }
        }
    }

    LOG_ERROR_TAG(KV_LOG_TAG, "%s end\n", __FUNCTION__);
}

int kv_test(void)
{
    int ret = KV_OK;
    memset(&store_key, 0, sizeof(store_key));
    arch_os_mutex_create(&kv_test_store_mutex);
    int num = 0;

#if (CONFIG_KV_TEST_TEST_CASE_NUM == 1)
    if (KV_OK != arch_os_thread_create(&kv_test_thread, "kv_t_Task", kv_test_proc, CONFIG_EXAMPLES_KV_TEST_STACKSIZE,
                                         NULL, ARCH_OS_PRIORITY_DEFAULT)) {
        LOG_WARN_TAG(KV_LOG_TAG, "kv test thread create failed");
        ret = KV_ERROR;
    }
#endif

#if (CONFIG_KV_TEST_TEST_CASE_NUM == 2)
    if (KV_OK != arch_os_thread_create(&kv_test_thread2, "kv_t_Task_2", kv_test2_proc,
                                         CONFIG_EXAMPLES_KV_TEST_STACKSIZE, NULL, ARCH_OS_PRIORITY_DEFAULT)) {
        LOG_WARN_TAG(KV_LOG_TAG, "kv test thread2 create failed");
        ret = KV_ERROR;
    }
#endif

#if (CONFIG_KV_TEST_TEST_CASE_NUM == 3)
    if (KV_OK != arch_os_thread_create(&kv_test_thread, "kv_t_Task", kv_test_proc, CONFIG_EXAMPLES_KV_TEST_STACKSIZE,
                                         NULL, ARCH_OS_PRIORITY_DEFAULT)) {
        LOG_WARN_TAG(KV_LOG_TAG, "kv test thread create failed");
        ret = KV_ERROR;
    }

    if (KV_OK != arch_os_thread_create(&kv_test_thread3, "kv_t_Task_3", kv_test3_proc,
                                         CONFIG_EXAMPLES_KV_TEST_STACKSIZE, NULL, ARCH_OS_PRIORITY_DEFAULT)) {
        LOG_WARN_TAG(KV_LOG_TAG, "kv test thread3 create failed");
        ret = KV_ERROR;
    }
#endif

#if (CONFIG_KV_TEST_TEST_CASE_NUM == 5)
    if (KV_OK != arch_os_thread_create(&kv_test_thread5, "kv_t_Task_5", kv_test5_proc,
                                         CONFIG_EXAMPLES_KV_TEST_STACKSIZE, NULL, ARCH_OS_PRIORITY_DEFAULT)) {
        LOG_WARN_TAG(KV_LOG_TAG, "kv test thread5 create failed");
        ret = KV_ERROR;
    }
#endif

    return ret;
}
