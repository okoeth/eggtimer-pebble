////////////////////////////////////////////////////////////
// Implementation of data structure 
//
#include "pebble.h"
#include "data.h"

#define MAX_DATA 32

#define BASE_DATA_ID 1000
#define COUNT_DATA_ID 900
#define VERSION_DATA_ID 900

#define VERSION_DATA 1

void  configuration_set_values (struct Configuration * pstConfig_p, uint32_t u32Height_p, uint32_t u32Weight_p, uint32_t u32StartTemp_p, uint32_t u32EndTemp_p) {
	if (pstConfig_p!=NULL) {
		if (u32EndTemp_p <= 67) {
			snprintf(pstConfig_p->szTitle, 32, "Soft - %lug", u32Weight_p);
		}	
		else if (u32EndTemp_p <= 77) {
			snprintf(pstConfig_p->szTitle, 32, "Medium - %lug", u32Weight_p);
		}
		else {
			snprintf(pstConfig_p->szTitle, 32, "Hard - %lug", u32Weight_p);
		}
		snprintf(pstConfig_p->szSubtitle, 32, "%lum / %luC / %luC", u32Height_p, u32StartTemp_p, u32EndTemp_p);
	}
}

static void data_write_to_store (struct Data * pstData_p) {
	persist_write_int(VERSION_DATA_ID, VERSION_DATA);
	persist_write_int(COUNT_DATA_ID, pstData_p->u16NumConfigs);
	for (int i = 0; i < pstData_p->u16NumConfigs; i++) {
		persist_write_data(BASE_DATA_ID+i, & (pstData_p->prgstConfig[i]), sizeof(struct Configuration));
	}
} 

static void data_read_from_store (struct Data * pstData_p) {
	if (persist_exists(VERSION_DATA_ID)) {
		// persist_read_int(VERSION_DATA_ID); -- currently not used
	}
	if (persist_exists(COUNT_DATA_ID)) {
		pstData_p->u16NumConfigs = persist_read_int(COUNT_DATA_ID);
		for (int i = 0; i < pstData_p->u16NumConfigs; i++) {
			if (persist_exists(BASE_DATA_ID+i)) {
				persist_read_data(BASE_DATA_ID+i, & (pstData_p->prgstConfig[i]), sizeof(struct Configuration));
			}
		}
	}
} 

struct Data *  data_create () {
	struct Data * pData = calloc(1, sizeof(struct Data));
	pData->u16NumConfigs=0;
	pData->prgstConfig=calloc(MAX_DATA, sizeof(struct Configuration));
	data_read_from_store(pData);
	return pData;
}

int16_t data_destroy (struct Data * pstData_p) {
	data_write_to_store(pstData_p);
	free(pstData_p->prgstConfig); 
	free(pstData_p);
	return DATA_RET_OK;
}

int16_t data_append_items (struct Data * pstData_p, struct Configuration * prgstConfig_p, uint16_t u16NumConfigs_p) {
	if ((pstData_p->u16NumConfigs + u16NumConfigs_p) > MAX_DATA) {
		return DATA_RET_OUT_OF_BOUNDS;
	}
	memcpy(& (pstData_p->prgstConfig[pstData_p->u16NumConfigs]), prgstConfig_p, sizeof(struct Configuration)*u16NumConfigs_p);
	pstData_p->u16NumConfigs+=u16NumConfigs_p;
	return DATA_RET_OK;
}

int16_t data_append_item (struct Data * pstData_p, struct Configuration * pstConfig_p) {
	if (pstData_p->u16NumConfigs + 1 > MAX_DATA) {
		return DATA_RET_OUT_OF_BOUNDS;
	}
	memcpy(& (pstData_p->prgstConfig[pstData_p->u16NumConfigs]), pstConfig_p, sizeof(struct Configuration)*1);
	pstData_p->u16NumConfigs+=1;
	return DATA_RET_OK;
}

int16_t data_remove_item (struct Data * pstData_p, uint16_t u16Pos_p) {
	if (pstData_p->u16NumConfigs <= u16Pos_p) {
		return DATA_RET_OUT_OF_BOUNDS;
	}
	for (int i=u16Pos_p; i < pstData_p->u16NumConfigs-1; i++) {
		memcpy(& (pstData_p->prgstConfig[i]), & (pstData_p->prgstConfig[i+1]), sizeof(struct Configuration));
	}
	pstData_p->u16NumConfigs-=1;
	return DATA_RET_OK;
}

int16_t data_remove_all_items (struct Data * pstData_p) {
	pstData_p->u16NumConfigs=0;
	return DATA_RET_OK;
}

int16_t data_log (struct Data * pstData_p) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Number of items: %u", pstData_p->u16NumConfigs);
	for (int i = 0; i < pstData_p->u16NumConfigs; i++) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "%s, %s", pstData_p->prgstConfig[i].szTitle, pstData_p->prgstConfig[i].szSubtitle);
	}
	return DATA_RET_OK;
}

int16_t data_config_log (struct Configuration * pstConfig_p) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%s, %s", pstConfig_p->szTitle, pstConfig_p->szSubtitle);
	return DATA_RET_OK;
}
