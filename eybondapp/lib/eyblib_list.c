/********************************Copyright I&S Team*****************************
 * ＊ 文件名： list.c
 * ＊ 描述 ： 链表操作原文件
 * ＊ 创建者： UnartyChen
 * ＊ 版本 ： V1.0
 * ＊ 日期 ： 2015.03.1
        2017.12.05 CGQ 修改相关库调用头
 * ＊ 修改 ： 暂无
 * ****************************************************************************/
#include "eyblib_list.h"
#include "eyblib_memory.h"

#define LIST_APPLY_FLAG     (0x55aa33cc)

static Node_t *nodeCheck(u8_t *pLoad);
static Node_t *nodeFind(ListHandler_t *head, u8_t *pLoad);

/*******************************************************************************
 * ＊ 函数名： list_init
 * ＊ 描述 ： 链表初始化函数
 * ＊ 参数 ： head: 目标链表头指针
 * ＊ 返回 ： 无
 * ****************************************************************************/
void list_init(ListHandler_t *head) {
  head->count = 0;
  head->node = NULL;
}

/*******************************************************************************
 * ＊ 函数名： list_nodeApply
 * ＊ 描述 ： 节点申请
 * ＊ 参数 ： size:       节点带载数据大小
 * ＊ 返回 ： 申请节点负载地址
 * ****************************************************************************/
void *list_nodeApply(mcu_t payloadSize) {
  Node_t *node = NULL;
  node = memory_apply(payloadSize + sizeof(Node_t));
  ERRR(node == NULL, return NULL);

  /*数据空间申请成功*/
  node->front = (Node_t *)(LIST_APPLY_FLAG ^ (int)node->payload);

  return node->payload;
}
/*******************************************************************************
 * ＊ 函数名： list_nodeDelete
 * ＊ 描述 ： 链表节点删除
 * ＊ 参数 ： node:   目标操作节点
 * ＊ 返回 ： 插入结果：FALSE／TRUE
 * ****************************************************************************/
void list_nodeDelete(ListHandler_t *head, void *payload) {
  Node_t *node = nodeCheck((u8_t *)payload);
  if (node != null) {
    memory_release(node);
    node = NULL;
    return;
  }

  ERRR((head == NULL || head->node == NULL), return);
  node = nodeFind(head, payload);
  if (node != null) {
    if (head->node == node) {
      if (node->next == head->node) { //最后一个节点
        head->node = null;
      } else {
        head->node = node->next;
      }
    }
    node->front->next = node->next;
    node->next->front = node->front;
    if (node != NULL) {
      memory_release(node);
      node = NULL;
    }
    head->count--;
  }
}

/*******************************************************************************
 * ＊ 函数名： list_delete
 * ＊ 描述 ： 链表删除
 * ＊ 参数 ： head:   目标操作链表头
 * ＊ 返回 ： 插入结果：FALSE／TRUE
 * ****************************************************************************/
void list_delete(ListHandler_t *head) {
  Node_t *node = NULL;
  Node_t *tail = head->node;

  ERRR((head == NULL || tail == NULL), return);
  do {
    node = tail;
    tail = tail->next;
    if (node != NULL) {
      memory_release(node);
      node = NULL;
    }
  } while (tail != head->node && tail != NULL);

  list_init(head);
}

/*******************************************************************************
 * ＊ 函数名： list_topInsert
 * ＊ 描述 ： 链表节点前插
 * ＊ 参数 ： head:   目标操作链表头指针
        payload:  目标插入数据
 * ＊ 返回 ： 插入结果：0: 成功  or Fail
 * ****************************************************************************/
int list_topInsert(ListHandler_t *head, void *payload) {
  int i = list_bottomInsert(head, payload);

  if (i == 0) {
    head->node = head->node->front;
  }

  return 0;
}

/*******************************************************************************
 * ＊ 函数名： list_bottomInsert
 * ＊ 描述 ： 链表节点前插
 * ＊ 参数 ： head:   目标操作链表头指针
        payload:  目标插入数据
 * ＊ 返回 ： 插入结果：0: 成功  or Fail
 * ****************************************************************************/
int list_bottomInsert(ListHandler_t *head, void *payload) {
  if (head == null) {
    return 1;
  }

  Node_t *node = nodeCheck((u8_t *)payload);
  if (node == null) {
    node = nodeFind(head, payload);
    if (node == null) {
      return 2;
    } else {
      node->front->next = node->next;
      node->next->front = node->front;
      head->count--;
    }
  }

  if (head->node == NULL) {
    head->node = node;
    node->front = node;
    node->next = node;
  } else {
    Node_t *tail = head->node;

    node->front = tail->front;
    node->next = head->node;
    tail->front->next = node;
    tail->front = node;
  }
  head->count++;

  return 0;
}


/*******************************************************************************
 * ＊ 函数名： list_ConfInsertNodeApply
 * ＊ 描述 ： 链表条件插入
 * ＊ 参数 ： head:   目标操作链表头指针
                cmp:      数据比较函数
        data:   节点内存空间地
          size:        数据长度
 * ＊ 返回 ： 节点数据存储首地址.  null；申请失败
 * ****************************************************************************/
int list_ConfInsert(ListHandler_t *head, cmpFun cmp, void *payload) {
  if (head == null) {
    return 1;
  }

  Node_t *node = nodeCheck((u8_t *)payload);
  if (node == null) {
    node = nodeFind(head, payload);
    if (node == null) {
      return 2;
    } else {
      node->front->next = node->next;
      node->next->front = node->front;
      head->count--;
    }
  }
  /*数据空间申请成功*/
  if (head->node == NULL) {
    head->node = node;
    node->front = node;
    node->next = node;
  } else {
    Node_t *tail = head->node;
    do {
      if (0 == cmp(tail->payload, payload)) {
        if (tail == head->node) { //头节点前插
          head->node = node;
        }
        break;
      }
      tail = tail->next;
    } while (tail != head->node);

    node->front = tail->front;
    node->next = tail;
    tail->front->next = node;
    tail->front = node;
  }
  head->count++;

  return 0;
}

/*******************************************************************************
 * ＊ 函数名： list_nextNode
 * ＊ 描述 ： 获取当前的下一个节点数据
 * ＊ 参数 ：
 * ＊ 返回 ： NULL(未查找到)/节点地址
 * ****************************************************************************/
void *list_nextData(ListHandler_t *head, void *payload) {
  Node_t *node = nodeFind(head, payload);

  if (node == null) {
    if (head != null && head->node != NULL) {
      return head->node->payload;
    }
  } else if (node->next != head->node) {
    return node->next->payload;
  }

  return null;
}

/*******************************************************************************
 * ＊ 函数名： list_find
 * ＊ 描述 ： 链表节点查找
 * ＊ 参数 ： cmp:  数据比较函数
 *        data: 目标核对数据
 * ＊ 返回 ： NULL(未查找到)/节点地址
 * ****************************************************************************/
void *list_find(ListHandler_t *head, cmpFun cmp, void *conVal) {
  Node_t *tail = head->node;

  ERRR((head == NULL || tail == NULL), return NULL);  // mike 20200826
  do {
    if (0 == cmp(tail->payload, conVal)) {
      return tail->payload;
    }
    tail = tail->next;
  } while (tail != head->node);

  return null;
}
/*******************************************************************************
 * ＊ 函数名： list_trans
 * ＊ 描述 ： 链表遍历
 * ＊ 参数 ： head:   目标操作链表头
                fun:        数据操作函数
                optPoint    外部数据指针
 * ＊ 返回 ： 插入结果：FALSE／TRUE
 * ****************************************************************************/
void list_trans(ListHandler_t *head, processorFun fun, void *optPoint) {
  Node_t *tail = head->node;
  void *payload = NULL;

  ERRR((head == NULL || tail == NULL || fun == NULL), return);
  do {
    payload = tail->payload;
    tail = tail->next;
    if (0 == fun(payload, optPoint)) {
      break;
    }
  } while (tail != head->node && head->node != NULL);
}

/*******************************************************************************
 * ＊ 函数名： nodeCheck
 * ＊ 描述 ： 核对数据空间是否正常通过链表审请的
 * ＊ 参数 ： pLoad  目标数据空间
 * ＊ 返回 ： 插入结果：FALSE／TRUE
 * ****************************************************************************/
static Node_t *nodeCheck(u8_t *pLoad) {
  Node_t *node = (Node_t *)(((u8_t *)pLoad) - sizeof(Node_t));

  if (pLoad != null && (LIST_APPLY_FLAG ^ (int)node->front) == (int)pLoad) {
    return node;
  }

  return null;
}

/*******************************************************************************
 * ＊ 函数名： nodeFind
 * ＊ 描述 ： 核对数据空间是否处于链表中
 * ＊ 参数 ： head:   目标操作链表头
 * ＊ 返回 ： 插入结果：FALSE／TRUE
 * ****************************************************************************/
static Node_t *nodeFind(ListHandler_t *head, u8_t *pLoad) {
  ERRR((head == null || head->node == NULL || pLoad == NULL), return NULL);

  Node_t *node = (Node_t *)(((u8_t *)pLoad) - sizeof(Node_t));
  Node_t *tail = head->node;
  do {
    if (node == tail) {
      return node;
    }
    tail = tail->next;
  } while (tail != head->node && tail != NULL);

  return null;
}

/*****************************Copyright I＆S Team******************************/
