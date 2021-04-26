typedef struct {
    size_t length;
    size_t used;
    char *data;
} strbuf_t;

int sb_init(strbuf_t *, size_t);
int sb_insert(strbuf_t *list, int index, char item);
void sb_destroy(strbuf_t *list);
int sb_append(strbuf_t *, char);
int sb_remove(strbuf_t *, char *);
int sb_concat(strbuf_t *sb, char *str);