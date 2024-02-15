/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT(8의 배수) */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

/* size_t 변수가 차지하는 메모리 공간 크기를 8바이트 경계에 맞출 수 있도록 조정 */
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*기본 상수들과 매크로들*/
#define WSIZE 4 // word와 header/footer size (bytes)
#define DSIZE 8 // Double word size (bytes)
#define CHUNKSIZE (1<<12) // 힙 확장을 위한 기본 크기(=초기 빈블록의 크기=4096)
#define MAX(x, y) ((x) > (y)? (x) : (y)) //최대값 찾는 함수

/*size에 할당 여부 비트를 비트 OR연산을 통해 최종적인 헤더 값을 리턴한다.*/ 
#define PACK(size, alloc) ((size) | (alloc))

/*p가 참조하는 워드 반환*/
#define GET(p) (*(unsigned int *)(p))

/*p에 val 저장*/
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/*사이즈 (~0x7: ...11111000, '&' 연산으로 마지막 세자리 0으로 바꿈*/
#define GET_SIZE(p) (GET(p) & ~0x7) // 헤더 값에서 사이즈를 리턴하는 함수

/*할당 상태*/
/*마지막 자리를 제외하고 모두 0으로 바꿈*/
/*할당이 되어 있다면 마지막 자리가 1로 반환되고, 안 되어 있다면 마지막 자리가 0으로 반환됨*/
#define GET_ALLOC(p) (GET(p) & 0x1) // 헤더 값에서 할당 여부를 리턴하는 함수

/*해당 블록의 헤더주소 반환*/
#define HDRP(bp) ((char *)(bp) - WSIZE)

/*해당 블록의 푸터주소 반환*/
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/*다음 블록 포인터(페이로드)를 리턴(해당 블록 페이로드 포인터에 블록크기 더함)*/
/* GET_SIZE(((char *)(bp)-WSIZE))는 현재 블록의 헤더에 있는 사이즈 정보를 읽어옴*/
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))

/*이전 블록 포인터(페이로드)를 리턴(해당 블록 페이로드 포인터에 이전 블록크기 뺌)*/
/* GET_SIZE((char *)(bp)-DSIZE)는 이전 블록의 footer에 있는 사이즈 정보를 읽어옴*/
#define PREV_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))

/* 힙의 시작 지점을 가리키는 포인터*/
static void *heap_listp;
/* 힙 메모리 영역 확장하기*/
static void *extend_heap(size_t words);
/* 가용 블록 연결하기*/
static void *coalesce(void *bp);
/* 가용 블록 검색하기 (first-fit) */
static void *find_fit(size_t asize);
/* 할당된 블록 배치하기*/
static void place(void *bp, size_t asize);

/* mm_init - initialize the malloc package. */
int mm_init(void)
{
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 * Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














