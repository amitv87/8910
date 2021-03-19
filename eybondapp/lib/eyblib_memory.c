/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
 * ＊ 文件名： eyblib_memory.c
 * ＊ 描述 ： 内存操作实现原码
 * ＊ 创建者： UnartyChen
 * ＊ 版本 ： V1.0
 * ＊ 日期 ： 2015.03.12
 * ＊ 修改 ： 2017.03.17 Unarty
                更新字节对齐方法 line = 50. 移除内存起始边界判断，
                修改内存标记方法，MEMORY_FLAG^areaSize
                2017.09.05 Unarty
                更新分配Bug, 当可分配区域 n - size < sizeof(MemArea_t) 时, 会出现下下结点内存值复盖
                修改分配内存标记方法。
                2019.03.26 Unarty
                内存释放增加地址边界对齐判断，某些设备内存非对齐访问时会导致硬件异常。
 * ****************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_memory.h"
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eyblib_memory.h"

// #define MEMORY_FLAG   (0xA55A55AA)//((void*)0x4885A112)
// #define MEMORY_ALIGNMENT (4)                //内存最小边界对齐

/* typedef struct memArea {
  struct memArea  *nextArea;
  mcu_t     areaSize;
} MemArea_t;  //内存区域块描述 */

// static MemArea_t *mem;

/*******************************************************************************
 * ＊ 函数名： memory_init
 * ＊ 描述 ： 内存申请分配空间初始化
 * ＊ 参数 ： memAddr:  动态内存分配的地址
        memSize:  动态内存分配的空间
 * ＊ 返回 ： 无
 * ＊ 注意 ： 无
 * ****************************************************************************/
void memory_init(void *memAddr, mcu_t memSize) {
/*  mem = (MemArea_t *)memAddr;
  mem->areaSize = memSize;
  mem->nextArea = null; */
}

/*******************************************************************************
 * ＊ 函数名： memory_apply
 * ＊ 描述 ： 内存申请
 * ＊ 参数 ： size: 空间大小
 * ＊ 返回 ： 申请的地址，null（申请失败）／有效内存地址（申请成功)
 * ****************************************************************************/
void *memory_apply(mcu_t size) {
/*   MemArea_t *tail, *pion;
  void *memAddr;

  // 字节对齐
  size += sizeof(MemArea_t) + (MEMORY_ALIGNMENT - 1);
  size &= ~(MEMORY_ALIGNMENT - 1);

  for (pion = (MemArea_t *)&mem, tail = mem; tail != null; pion = tail, tail = tail->nextArea) {
    if (tail->areaSize >= size) {
      memAddr = tail;
      if ((tail->areaSize - size) < (sizeof(MemArea_t) + MEMORY_ALIGNMENT)) {
        pion->nextArea = tail->nextArea;
        size = tail->areaSize;
      } else {
        tail = (MemArea_t *)((u8_t *)memAddr + size);
        tail->areaSize = ((MemArea_t *)memAddr)->areaSize - size;
        tail->nextArea = ((MemArea_t *)memAddr)->nextArea;
        pion->nextArea = tail;
        ((MemArea_t *)memAddr)->areaSize = size;
      }

      ((MemArea_t *)memAddr)->nextArea = (void *)(MEMORY_FLAG ^ size);

      return ((u8_t *)memAddr + sizeof(MemArea_t));
    }
  } */
#ifdef _PLATFORM_BC25_
  return Ql_MEM_Alloc(size);
#endif

#ifdef _PLATFORM_L610_
  return fibo_malloc(size);
#endif
  return null;
}
/*******************************************************************************
 * ＊ 函数名： memory_release
 * ＊ 描述 ： 内存释放
 * ＊ 参数 ： addr: 目标释放地址
 * ＊ 返回 ： 无
 * ****************************************************************************/
void memory_release(void *addr) {
/*   if (null != addr && (((int)addr & (MEMORY_ALIGNMENT - 1)) == 0)) {
    MemArea_t *tail, *pion;
    MemArea_t *rlsArea = (MemArea_t *)((mcu_t)addr - sizeof(MemArea_t));

    if ((void *)(rlsArea->areaSize ^ MEMORY_FLAG) == rlsArea->nextArea) {
      // 找到插入节点
      for (pion = (MemArea_t *)&mem, tail = mem; (tail < rlsArea) && (tail != null); pion = tail, tail = tail->nextArea)
      {}

      rlsArea->nextArea = tail;
      pion->nextArea = rlsArea;
      if (pion != (MemArea_t *)&mem) { //目标不处于头节点
        rlsArea = pion;
      }
      while (rlsArea != null &&  rlsArea < tail) {
        if (((u8_t *)(rlsArea) + rlsArea->areaSize) == (u8_t *)rlsArea->nextArea) {
          rlsArea->areaSize += rlsArea->nextArea->areaSize;
          rlsArea->nextArea = rlsArea->nextArea->nextArea;
        } else {
          rlsArea = rlsArea->nextArea;
        }
      }
    }
  } */
#ifdef _PLATFORM_BC25_
  if (addr != NULL) {
    Ql_MEM_Free(addr);
  }
#endif
    
#ifdef _PLATFORM_L610_
  if (addr != NULL) {
    fibo_free(addr);
  }
#endif
  addr = NULL;
}

/******************************************************************************/

