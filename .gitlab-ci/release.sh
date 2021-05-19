#!/bin/bash
set -eu

VERSION="$(echo "${CI_COMMIT_TAG}" | sed -nE 's/v([0-9]+)\.([0-9]+)\.([0-9]+).*/\1.\2.\3/p')"

declare -a args
for dist in logc-*.tar.gz logc-*.tar.xz logc-*.zip; do
	URL="${CI_API_V4_URL}/projects/${CI_PROJECT_ID}/packages/generic/logc/${VERSION}/${dist}"
	curl --header "JOB-TOKEN: ${CI_JOB_TOKEN}" --upload-file "${dist}" "${URL}"
	args+=("--assets-link" "{\"name\":\"${dist}\",\"url\":\"${URL}\"}")
done

release-cli create \
	--name "Release ${CI_COMMIT_TAG#v}" \
	--tag-name "$CI_COMMIT_TAG" \
	--description "$(sed -n '/^## /,/^## /{n;/^## /q;p}' CHANGELOG.md)" \
	"${args[@]}"
