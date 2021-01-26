#ifndef X_STRING_H
#define X_STRING_H

#include <stdarg.h>

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define GNUC_PRINTF_CHECK(fmt_idx, arg_idx) __attribute__((format (printf, fmt_idx, arg_idx)))
#else
#define GNUC_PRINTF_CHECK(fmt_idx, arg_idx)
#endif

#define _S(SB) string_raw((SB))

struct xstring;

struct xstring *string_new(size_t reserve);

void string_free(struct xstring *sb);

void string_clear(struct xstring *sb);

char *string_detach(struct xstring *sb);

void string_move(struct xstring *src, struct string *dest);

void string_dup(struct xstring *src, struct string *dest);

void string_putc(struct xstring *sb, const char c);

void string_cat(struct xstring *sb, const char *str);

void string_ncat(struct xstring *sb, const char *str, size_t len);

void string_bcat(struct xstring *sb, const struct string *buf);

void string_catf(struct xstring *sb, const char *fmt, ...) GNUC_PRINTF_CHECK(2, 3);

int string_cmp(struct xstring *sb, const char *str);

int string_ncmp(struct xstring *sb, const char *str, size_t len);

int string_bcmp(struct xstring *sb, const struct string *buf);

const char *string_raw(struct xstring *sb);

size_t string_len(struct xstring *sb);

#endif  //X_STRING_H
