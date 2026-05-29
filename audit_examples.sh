#!/bin/bash
# Audit all Demi example programs — run with -d, check for CRITICAL/ERROR in stderr
set -o pipefail

cd /home/bobrossrtx/demi
ENGINE="./bin/demi-engine-debug"
OUTDIR="/home/bobrossrtx/notes/demi"
REPORT="$OUTDIR/example_audit_2026-05-29.md"

PASS=0
FAIL=0
WARN=0

echo "# Demi Engine Example Audit — 2026-05-29" > "$REPORT"
echo "" >> "$REPORT"
echo "Each example assembled and run in debug mode (-d)." >> "$REPORT"
echo "Checked for CRITICAL/ERROR lines in stderr and non-zero exit codes." >> "$REPORT"
echo "" >> "$REPORT"
echo "| Status | File | Stderr Issues | Notes |" >> "$REPORT"
echo "|--------|------|---------------|-------|" >> "$REPORT"

run_one() {
    local f="$1"
    local rel="${f#examples/}"
    local stderr_tmp=$(mktemp)
    local stdout_tmp=$(mktemp)

    local start=$(date +%s%N)
    timeout 10 "$ENGINE" -d -A "$f" >"$stdout_tmp" 2>"$stderr_tmp"
    local rc=$?
    local end=$(date +%s%N)
    local ms=$(( (end - start) / 1000000 ))

    local crit="./mnt/crit"   # dummy for grep
    local stderr_issues=""
    local crit_count=0
    local err_count=0

    if [ -s "$stderr_tmp" ]; then
        crit_count=$(grep -ac '\[CRITICAL\]' "$stderr_tmp" 2>/dev/null || echo 0)
        err_count=$(grep -ac '\[ERROR\]' "$stderr_tmp" 2>/dev/null || echo 0)
        if [ "$crit_count" -gt 0 ] || [ "$err_count" -gt 0 ]; then
            stderr_issues="${crit_count}C/${err_count}E"
        fi
    fi

    local status="OK"
    local notes=""

    if [ "$rc" -ne 0 ]; then
        status="FAIL"
        notes="exit=$rc"
        FAIL=$((FAIL+1))
    elif [ -n "$stderr_issues" ]; then
        # Check if these are expected (e.g. program prints text containing "Error:")
        # Look for patterns that are false positives
        local false_pos=0
        if [ -s "$stdout_tmp" ] && grep -q 'Error:\|error:\|ERROR' "$stdout_tmp"; then
            # stdout contains "Error:" text — likely program output, not a real error
            false_pos=1
        fi
        if grep -q 'calc>\|Error: Invalid\|Error: Division' "$stderr_tmp" 2>/dev/null; then
            false_pos=1
        fi

        if [ "$false_pos" -eq 1 ]; then
            status="WARN"
            notes="false positive (program prints 'Error:' or similar as normal output)"
            WARN=$((WARN+1))
        else
            status="ISSUE"
            notes=$(head -3 "$stderr_tmp" | tr '\n' ' ' | sed 's/|/\\|/g')
            FAIL=$((FAIL+1))
        fi
    else
        PASS=$((PASS+1))
    fi

    printf "| %-6s | %-55s | %-13s | %s |\n" "$status" "$rel" "${stderr_issues:-none}" "${notes:--}" >> "$REPORT"

    rm -f "$stderr_tmp" "$stdout_tmp"
}

for f in $(find examples -name '*.asm' | sort); do
    run_one "$f"
done

echo "" >> "$REPORT"
echo "## Summary" >> "$REPORT"
echo "- PASS: $PASS  (clean run, no issues)" >> "$REPORT"
echo "- WARN: $WARN  (false positives — program text contains 'Error' etc.)" >> "$REPORT"
echo "- FAIL: $FAIL  (real issues — CRITICAL/ERROR in debug log or non-zero exit)" >> "$REPORT"
echo "- Total: $((PASS + WARN + FAIL))" >> "$REPORT"

cat "$REPORT"
