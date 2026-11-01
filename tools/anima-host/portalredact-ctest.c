// Host test for the Evil Portal password-redaction helper (firmware/components/nucleo_evilportal/
// include/nucleo_evilportal_redact.h). Compiled + run by portalredact-check.mjs — no ESP-IDF. Proves
// the four loot redaction modes (full / mask / hidden / length-only) and the boundary cases (empty
// input, 1-2 char secrets, tiny output buffers) BEFORE the engine writes anything to SD.
// FOR AUTHORIZED TESTING ONLY — string logic only, no capture happens here.
#include "nucleo_evilportal_redact.h"
#include <stdio.h>
#include <string.h>

static int g_pass = 0, g_fail = 0;
#define EQ(got, want, msg) do { \
    if (strcmp((got), (want)) == 0) { g_pass++; } \
    else { g_fail++; printf("  FAIL: %s  got=\"%s\" want=\"%s\"  (%s:%d)\n", msg, (got), (want), __FILE__, __LINE__); } } while (0)

int main(void)
{
    printf("== nucleo_evilportal_redact host test ==\n");
    char o[64];

    // FULL: verbatim.
    ep_redact_pass("Estate2024!", EP_PWD_FULL, o, sizeof o); EQ(o, "Estate2024!", "full verbatim");
    ep_redact_pass("",            EP_PWD_FULL, o, sizeof o); EQ(o, "",            "full empty");

    // MASK: first + last kept, middle starred.
    ep_redact_pass("password",    EP_PWD_MASK, o, sizeof o); EQ(o, "p******d",    "mask 8");
    ep_redact_pass("hunter2",     EP_PWD_MASK, o, sizeof o); EQ(o, "h*****2",     "mask 7");
    ep_redact_pass("abc",         EP_PWD_MASK, o, sizeof o); EQ(o, "a*c",         "mask 3");
    ep_redact_pass("ab",          EP_PWD_MASK, o, sizeof o); EQ(o, "ab",          "mask 2 -> verbatim");
    ep_redact_pass("x",           EP_PWD_MASK, o, sizeof o); EQ(o, "x",           "mask 1 -> verbatim");
    ep_redact_pass("",            EP_PWD_MASK, o, sizeof o); EQ(o, "",            "mask empty");

    // HIDE: fixed label, but empty stays empty.
    ep_redact_pass("whatever",    EP_PWD_HIDE, o, sizeof o); EQ(o, "*hidden*",    "hide label");
    ep_redact_pass("",            EP_PWD_HIDE, o, sizeof o); EQ(o, "",            "hide empty stays empty");

    // LEN: count + " chars".
    ep_redact_pass("hunter2",     EP_PWD_LEN,  o, sizeof o); EQ(o, "7 chars",     "len 7");
    ep_redact_pass("0123456789a", EP_PWD_LEN,  o, sizeof o); EQ(o, "11 chars",    "len 11 (two digits)");
    ep_redact_pass("",            EP_PWD_LEN,  o, sizeof o); EQ(o, "",            "len empty stays empty");

    // Out-of-range mode -> treated as FULL.
    ep_redact_pass("secret",      99,          o, sizeof o); EQ(o, "secret",      "bad mode -> full");

    // Tiny output buffer never overflows and always NUL-terminates.
    { char t[4]; ep_redact_pass("password", EP_PWD_FULL, t, sizeof t); EQ(t, "pas", "tiny buf full truncates"); }
    { char t[4]; ep_redact_pass("password", EP_PWD_HIDE, t, sizeof t); EQ(t, "*hi", "tiny buf hide truncates"); }
    { char t[1]; t[0] = 'Z'; ep_redact_pass("password", EP_PWD_FULL, t, sizeof t); EQ(t, "", "buf==1 -> empty"); }

    // NULL input is safe.
    ep_redact_pass(NULL, EP_PWD_FULL, o, sizeof o); EQ(o, "", "null input safe");

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail ? 1 : 0;
}
