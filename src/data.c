////////////////////////////////////////////////////////////
// Implementation of data structure 
//
#include "pebble.h"
#include "data.h"

#define MAX_DATA 32

struct Data *  data_create () {
	struct Data * pData = calloc(1, sizeof(struct Data));
	pData->u16NumConfigs=0;
	pData->prgstConfig=calloc(MAX_DATA, sizeof(struct Configuration));
	return pData;
}

int16_t data_destroy (struct Data * pstData_p) {
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
