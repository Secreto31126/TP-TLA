#! /bin/bash

set -u

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

GREEN='\033[0;32m'
RED='\033[0;31m'
OFF='\033[0m'
STATUS=0

echo "Compiler should accept..."
echo ""

accept_folder="src/test/c/accept/"
for test in $(find "$accept_folder" -type f -not -path '*/.*'); do
	cat "$test" | build/Compiler >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" == "0" ]; then
		echo -e "    ${test#$accept_folder}, ${GREEN}and it does${OFF} (status $RESULT)"
	else
		STATUS=1
		echo -e "    ${test#$accept_folder}, ${RED}but it rejects${OFF} (status $RESULT)"
	fi
done
echo ""

echo "Compiler should reject..."
echo ""

reject_folder="src/test/c/reject/"
for test in $(find "$reject_folder" -type f -not -path '*/.*'); do
	cat "$test" | build/Compiler >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" != "0" ]; then
		echo -e "    ${test#$reject_folder}, ${GREEN}and it does${OFF} (status $RESULT)"
	else
		STATUS=1
		echo -e "    ${test#$reject_folder}, ${RED}but it accepts${OFF} (status $RESULT)"
	fi
done
echo ""

echo "All done."
exit $STATUS
