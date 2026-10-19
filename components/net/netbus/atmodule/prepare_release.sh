#!/usr/bin/env bash

set -euo pipefail
shopt -s extglob
trap 'exit 1' ERR

usage() {
    printf '%s\n' \
        "Usage: $0 TAG PREVIOUS_OR_- NOTES_FILE" \
        'Squash the current source snapshot into a tagged local release branch.' \
        'Build and include English and Chinese HTML documentation.' \
        'Exclude this script from the release snapshot.' \
        'TAG: new public release tag.' \
        'PREVIOUS_OR_-: previous public tag, or - for the first release.' \
        'NOTES_FILE: reviewed Markdown notes, preferably outside the worktree.' \
        'Switches to release and prints a manual push command; never pushes.'
}

fail() {
    printf 'Release preparation failed: %s\n' "$*" >&2
    exit 1
}

if [[ $# == 1 && ($1 == --help || $1 == -h) ]]; then
    usage
    exit 0
fi
[[ $# == 3 ]] || { usage >&2; exit 2; }
tag=$1
previous=$2
notes_file=$3
root=$(git rev-parse --show-toplevel)
[[ $notes_file == /* ]] || notes_file=$PWD/$notes_file
[[ -f $notes_file ]] || fail "release notes file not found: $notes_file; create a reviewed Markdown notes file there"
notes=$(cat -- "$notes_file")
notes=${notes##+([[:space:]])}
notes=${notes%%+([[:space:]])}
cd -- "$root"
status=$(git --no-optional-locks status --porcelain --untracked-files=all)
[[ -z $status ]] ||
    fail 'commit or remove all worktree/index changes before preparing a release'
source_branch=$(git rev-parse --abbrev-ref HEAD)
[[ $source_branch != release ]] || fail 'switch to the development branch before preparing another release'
source=$(git rev-parse --verify 'HEAD^{commit}')
ref=refs/tags/$tag
git check-ref-format "$ref"
existing_refs=$(git for-each-ref '--format=%(refname)' "$ref")
if [[ $'\n'"$existing_refs"$'\n' == *$'\n'"$ref"$'\n'* ]]; then
    fail "tag already exists: $tag"
fi
[[ -n $notes ]] || fail 'release notes must not be empty'

parent=
parents=()
old_changelog=
if [[ $previous != - ]]; then
    previous_ref=refs/tags/$previous
    git check-ref-format "$previous_ref"
    parent=$(git rev-parse --verify "$previous_ref^{commit}")
    parents=(-p "$parent")
    changelog_entry=$(git ls-tree "$parent" -- CHANGELOG.md)
    if [[ -n $changelog_entry ]]; then
        old_changelog=$(git show "$parent:CHANGELOG.md")
    fi
fi
release_commit=$(git branch --list '--format=%(objectname)' release)
[[ -z $release_commit || $release_commit == "$parent" ]] ||
    fail 'local release must match the previous public tag before preparing a release'
worktrees=$(git worktree list --porcelain)
if [[ $'\n'"$worktrees"$'\n' == *$'\nbranch refs/heads/release\n'* ]]; then
    fail 'local release is checked out; switch that worktree to another branch before preparing a release'
fi
if [[ $old_changelog == '# Changelog'$'\n'* ]]; then
    old_changelog=${old_changelog#*$'\n'}
    old_changelog=${old_changelog##+($'\n')}
fi
today=$(date +%F)
changelog=$(printf '# Changelog\n\n## %s — %s\n\n%s\n' "$tag" "$today" "$notes")
[[ -z $old_changelog ]] || changelog+=$'\n\n'"$old_changelog"

# Check identity before writing objects; build the release tree in a private index.
git var GIT_AUTHOR_IDENT >/dev/null
git var GIT_COMMITTER_IDENT >/dev/null
temporary=$(mktemp -d -t atmodule-release-XXXXXXXX)
restore_docs=0
trap '
    if ((restore_docs)); then
        { rm -rf -- docs/build && mv -- "$temporary/old-docs" docs/build; } ||
            fail "could not restore docs/build; backup preserved in $temporary/old-docs"
    fi
    rm -rf -- "$temporary"
' EXIT
git archive "$source" docs | tar -x -C "$temporary"
SPHINXOPTS="${SPHINXOPTS:-} -D html_copy_source=0 -D html_show_sourcelink=0" "$temporary/docs/build_all.sh"
rm -f -- "$temporary/docs/build/en/html/.buildinfo" "$temporary/docs/build/zh/html/.buildinfo"
GIT_INDEX_FILE=$temporary/index git read-tree "$source"
GIT_INDEX_FILE=$temporary/index git update-index --force-remove -- prepare_release.sh
GIT_INDEX_FILE=$temporary/index GIT_WORK_TREE=$temporary git add -f -- docs/build/en/html docs/build/zh/html
blob=$(printf '%s\n' "$changelog" | git hash-object -w --stdin)
GIT_INDEX_FILE=$temporary/index git update-index --add --cacheinfo "100644,$blob,CHANGELOG.md"
tree=$(GIT_INDEX_FILE=$temporary/index git write-tree)
if [[ -e docs/build || -L docs/build ]]; then
    if ! mv -- docs/build "$temporary/old-docs"; then
        trap - EXIT
        fail "could not move docs/build; check docs/build and $temporary/old-docs"
    fi
    restore_docs=1
fi
commit=$(printf 'Release %s\n\n%s\n' "$tag" "$notes" | git commit-tree "$tree" "${parents[@]}")
git tag -a -m "Release $tag" -- "$tag" "$commit"
if ! git checkout --no-overwrite-ignore -B release "$commit"; then
    git tag -d -- "$tag" >/dev/null
    fail 'could not check out release'
fi
restore_docs=0
printf 'Release commit: %s\nLocal tag: %s\nChecked out local branch: release\n' "$commit" "$tag"
printf 'Manual push (replace github with your public remote):\n'
printf 'git push --atomic github release %q\n' "$ref"
