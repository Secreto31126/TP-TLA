#! /bin/bash

set -u

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

GREEN='\033[0;32m'
RED='\033[0;31m'
GREY='\033[1;30m'
OFF='\033[0m'
STATUS=0

test_count=0
pass_count=0
fail_count=0
skip_count=0

echo "Compiler should accept..."
echo ""

accept_folder="src/test/c/accept/"
for test in $(find "$accept_folder" -type f); do
	test_count=$((test_count + 1))

	if [[ "$(basename "$test")" == .skip* ]]; then
		skip_count=$((skip_count + 1))
		echo -e "    ${test#$accept_folder}, ${GREY}skipped${OFF}"
		continue
	fi

	cat "$test" | build/Compiler >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" == "0" ]; then
		echo -e "    ${test#$accept_folder}, ${GREEN}and it does${OFF} (status $RESULT)"
		pass_count=$((pass_count + 1))
	else
		STATUS=1
		echo -e "    ${test#$accept_folder}, ${RED}but it rejects${OFF} (status $RESULT)"
		fail_count=$((fail_count + 1))
	fi
done
echo ""

echo "Compiler should reject..."
echo ""

reject_folder="src/test/c/reject/"
for test in $(find "$reject_folder" -type f); do
	test_count=$((test_count + 1))

	if [[ "$(basename "$test")" == .skip* ]]; then
		skip_count=$((skip_count + 1))
		echo -e "    ${test#$accept_folder}, ${GREY}skipped${OFF}"
		continue
	fi

	cat "$test" | build/Compiler >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" != "0" ]; then
		echo -e "    ${test#$reject_folder}, ${GREEN}and it does${OFF} (status $RESULT)"
		pass_count=$((pass_count + 1))
	else
		STATUS=1
		echo -e "    ${test#$reject_folder}, ${RED}but it accepts${OFF} (status $RESULT)"
		fail_count=$((fail_count + 1))
	fi
done
echo ""

echo "All done."
echo -e "Executed $GREEN$test_count$OFF tests, $GREEN$pass_count$OFF passed, $RED$fail_count$OFF failed, $GREY$skip_count$OFF skipped."
exit $STATUS
