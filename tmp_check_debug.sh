cd /home/bobrossrtx/demi
for f in \
  examples/x64/advanced/fibonacci.asm \
  examples/x64/advanced/factorial_recursive.asm \
  examples/x64/features/core_instructions.asm \
  examples/x64/io/decimal_output.asm \
  examples/x86/data/labels_and_strings.asm; do
  echo "--- $f ---"
  count=$(./bin/demi-engine-debug -d -A "$f" 2>&1 | grep -ic "critical\|error\|fail")
  if [ "$count" -eq 0 ]; then
    echo "  clean"
  else
    echo "  $count matches"
    ./bin/demi-engine-debug -d -A "$f" 2>&1 | grep -i "critical\|error\|fail"
  fi
done
