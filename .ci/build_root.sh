set -evx

pushd "${DEPS_DIR}"
ROOT_URL="https://root.cern.ch/download/root_v6.16.00.Linux-ubuntu16-x86_64-gcc5.4.tar.gz"

if [[ ! -f "${DEPS_DIR}/root/bin/root-config" ]] ; then
  echo "Downloading Root"
  mkdir -p root
  travis_retry wget --no-check-certificate --quiet -O - "${ROOT_URL}" | tar --strip-components=1 -xz -C root
fi

source "${DEPS_DIR}/root/bin/thisroot.sh"
popd

set +evx
