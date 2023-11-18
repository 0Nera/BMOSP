#!/bin/sh

echo $(git log --pretty=format:%s ${LAST}..HEAD | grep ':' | sed -re 's/([^:]*)\:/- \`\1\`\:/' | sort)