// Pure password-redaction helper for the Evil Portal loot sinks — no ESP-IDF, no networking, so it
// compiles into both the firmware and the host gate (portalredact-ctest.c). It decides how a captured
// secret is written to the SD CSV and shown in the live UI. FOR AUTHORIZED TESTING ONLY — string
// processing only, no capture happens here.
//
// Rationale: Bruce lets the operator choose how much of a captured password is stored/shown (full,
// partially masked, fully hidden, or length-only) — useful for security-awareness demos where you
// want to PROVE a credential was captured without the room reading the actual secret off the screen.
#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

// Keep these values stable: they are persisted in NVS and crossed to the app UI.
enum {
    EP_PWD_FULL = 0,   // store/show the secret verbatim (default; matches prior behaviour)
    EP_PWD_MASK = 1,   // keep the first and last char, mask the middle: "p*****d"
    EP_PWD_HIDE = 2,   // never store/show the secret: "*hidden*"
    EP_PWD_LEN  = 3,   // store/show only its length: "8 chars"
    EP_PWD_MODE_N = 4,
};

// Write the redacted form of `in` into `out` (NUL-terminated, never exceeding outsz). `mode` is one
// of EP_PWD_*; an out-of-range mode falls back to EP_PWD_FULL. An empty input yields an empty string.
// Robust to a tiny `out` (always NUL-terminates) and never reads past `in`'s terminator.
static inline void ep_redact_pass(const char *in, int mode, char *out, size_t outsz)
{
    if (!out || outsz == 0) return;
    out[0] = 0;
    if (!in) return;
    size_t n = 0; while (in[n]) n++;                      // strlen without <string.h>

    if (mode == EP_PWD_HIDE) {
        if (n == 0) return;                               // empty stays empty (nothing was submitted)
        const char *h = "*hidden*";
        size_t i = 0; for (; h[i] && i + 1 < outsz; i++) out[i] = h[i];
        out[i] = 0; return;
    }
    if (mode == EP_PWD_LEN) {
        if (n == 0) return;
        // minimal unsigned->decimal, then " chars"
        char num[16]; int k = 0; size_t v = n;
        if (v == 0) num[k++] = '0';
        char tmp[16]; int t = 0; while (v) { tmp[t++] = (char)('0' + (v % 10)); v /= 10; }
        while (t) num[k++] = tmp[--t];
        num[k] = 0;
        const char *suf = " chars";
        size_t i = 0;
        for (int j = 0; num[j] && i + 1 < outsz; j++) out[i++] = num[j];
        for (int j = 0; suf[j]  && i + 1 < outsz; j++) out[i++] = suf[j];
        out[i] = 0; return;
    }
    if (mode == EP_PWD_MASK && n > 2) {
        size_t i = 0;
        if (i + 1 < outsz) out[i++] = in[0];
        for (size_t m = 1; m + 1 < n && i + 1 < outsz; m++) out[i++] = '*';
        if (i + 1 < outsz) out[i++] = in[n - 1];
        out[i] = 0; return;
    }
    // EP_PWD_FULL (or MASK on a <=2 char secret): copy verbatim, bounded.
    size_t i = 0; for (; in[i] && i + 1 < outsz; i++) out[i] = in[i];
    out[i] = 0;
}

#ifdef __cplusplus
}
#endif
