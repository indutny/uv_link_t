#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

#endif  /* SRC_COMMON_H_ */
