#ifndef DEFS_H
#define DEFS_H

#define OK 0x000
#define SUCCESS 0x100
#define FAIL 0x200

#define STATUS(x) x & 0xF00

#define CONTINUE (0x00 | OK)
#define EXIT (0x01 | OK)
#define NO_MEMORY (0x02 | FAIL)
#define OPTION_CANCEL (0x03 | OK)

#define EXTDATA_EXIST (0x20 | OK)
#define EXTDATA_NOT_EXIST (0x21 | FAIL)
#define EXTDATA_CREATE (0x22 | SUCCESS)
#define EXTDATA_NOT_CREATE (0x23 | FAIL)
#define EXTDATA_DELETE (0x24 | SUCCESS)
#define EXTDATA_NOT_DELETE (0x25 | FAIL)
#define EXTDATA_WRITE (0x26 | SUCCESS)
#define EXTDATA_NOT_OPEN (0x27 | FAIL)
#define EXTDATA_NOT_READ (0x28 | FAIL)
#define EXTDATA_NOT_WRITE (0x29 | FAIL)

#define SD_WRITE (0x40 | SUCCESS)
#define SD_NOT_OPEN (0x41 | FAIL)
#define SD_NOT_READ (0x42 | FAIL)
#define SD_NOT_WRITE (0x43 | FAIL)

#define STATUS_CASE(name, msg) \
    case name:                 \
        printf(msg "\n");      \
        break;

#endif
