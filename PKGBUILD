# Maintainer: Axel Wikström <axel.wikstrom@reaktor.com>

_pkgname=panel-pop
pkgname=panel-pop-git
pkgver=0.0.3.r0.g0000000
pkgrel=1
pkgdesc="A Panel de Pon / Tetris Attack / Puzzle League clone made with C++ and SDL"
arch=('x86_64')
url="https://github.com/a544jh/panel-pop"
# MIT covers panel-pop itself; the bundled fonts have their own terms, see
# THIRD-PARTY.md.
license=('MIT' 'OFL-1.1' 'LicenseRef-Freeware')
depends=('sdl2' 'sdl2_image' 'sdl2_ttf' 'sdl2_mixer')
makedepends=('git' 'cmake' 'boost')
provides=("$_pkgname")
conflicts=("$_pkgname")
source=("$_pkgname::git+https://github.com/a544jh/panel-pop.git")
sha256sums=('SKIP')

pkgver() {
	cd "$srcdir/$_pkgname"
	# Produces e.g. 0.0.3.r12.gabcdef0 (last tag + commits since + short hash).
	git describe --long --tags 2>/dev/null | sed 's/\([^-]*-g\)/r\1/;s/-/./g' ||
		printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
	cmake -B build -S "$_pkgname" \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX=/usr \
		-Wno-dev
	cmake --build build
}

package() {
	DESTDIR="$pkgdir" cmake --install build
	install -Dm644 "$_pkgname/LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
	# The bundled fonts are not MIT; ship their notices alongside it.
	install -Dm644 "$_pkgname/THIRD-PARTY.md" "$pkgdir/usr/share/licenses/$pkgname/THIRD-PARTY.md"
	install -Dm644 "$_pkgname/OFL.txt" "$pkgdir/usr/share/licenses/$pkgname/OFL.txt"
}
