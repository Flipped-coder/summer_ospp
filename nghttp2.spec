Name:           nghttp2
Version:        1.55.1
Release:        2
Summary:        Contains the HTTP/2 client, server and proxy programs.
License:        MIT
URL:            https://nghttp2.org/
Source0:        https://github.com/nghttp2/nghttp2/releases/download/v%{version}/%{name}-%{version}.tar.xz

Patch0:         backport-CVE-2023-44487.patch
Patch1:         backport-Fix-build-error-when-both-clock_gettime-and-GetTickCount64.patch

BuildRequires:  CUnit-devel c-ares-devel gcc-c++ libev-devel openssl-devel automake
BuildRequires:  python3-devel systemd-devel zlib-devel make libxml2-devel

Requires:       libnghttp2 = %{version}-%{release}
%{?systemd_requires}

%description
The framing layer of HTTP/2 is implemented as a form of reusable C library.
On top of that, we have implemented HTTP/2 client, server and proxy. We have
also developed load test and benchmarking tool for HTTP/2.

%package     -n libnghttp2
Summary:        %{name} - HTTP/2 C Library

%description -n libnghttp2
libnghttp2 is a library implementing HTTP/2 protocol in C.

%package     -n libnghttp2-devel
Summary:        includes development files for %{name}
Requires:       libnghttp2 = %{version}-%{release} pkgconfig

%description -n libnghttp2-devel
Files needed for building applications,such as static libraries,
header files for %{name}

%package_help

%prep
%autosetup -n %{name}-%{version} -p1
sed -e '1 s|^#!/.*python|&3|' -i script/fetch-ocsp-response

%build
autoreconf
%configure PYTHON=%{__python3} --disable-hpack-tools --disable-python-bindings\
    --with-libxml2 --disable-static
%disable_rpath
%make_build  V=1

%install
%make_install
install -d $RPM_BUILD_ROOT%{_unitdir}
install -p -m0444  contrib/nghttpx.service $RPM_BUILD_ROOT%{_unitdir}
%delete_la

%ldconfig_scriptlets -n libnghttp2

%post
%systemd_post nghttpx.service

%postun
%systemd_postun nghttpx.service

%check
export "LD_LIBRARY_PATH=$RPM_BUILD_ROOT%{_libdir}:$LD_LIBRARY_PATH"
%make_build check

%files
%defattr(-,root,root)
%license COPYING
%{_bindir}/h2load
%{_bindir}/nghttp*
%{_datadir}/nghttp2
%{_datadir}/doc/nghttp2/README.rst
%{_unitdir}/nghttpx.service

%files  -n libnghttp2
%defattr(-,root,root)
%license COPYING
%{_libdir}/libnghttp2.so.*
%{!?_licensedir:%global license %%doc}

%files  -n libnghttp2-devel
%defattr(-,root,root)
%doc README.rst
%{_includedir}/nghttp2
%{_libdir}/libnghttp2.so
%{_libdir}/pkgconfig/libnghttp2.pc

%files help
%defattr(-,root,root)
%{_mandir}/man1/*

%changelog
* Thu Oct 19 2023 xingwei <xingwei14@h-partners.com> - 1.55.1-2
- Type:CVE
- ID:CVE-2023-44487
- SUG:NA
- DESC:fix CVE-2023-44487 and build error

* Tue Jul 25 2023 wangye <wangye91@h-partners.com> - 1.55.1-1
- Type:requirements
- ID:NA
- SUG:NA
- DESC:update nghttp2 to 1.55.1

* Sat Jul 15 2023 wangye <wangye91@h-partners.com> - 1.52.0-2
- Type:cve
- ID:CVE-2023-35945
- SUG:NA
- DESC:fix CVE-2023-35945

* Mon Mar 06 2023 xinghe <xinghe2@h-partners.com> - 1.52.0-1
- Type:requirements
- Id:NA
- SUG:NA
- DESC:update nghttp2 to 1.52.0

* Tue Feb 28 2023 gaihuiying <eaglegai@163.com> - 1.47.0-3
- Type:bugfix
- ID:NA
- SUG:NA
- DESC:delete *.a file

* Wed Aug 31 2022 xingwei <xingwei14@h-partners.com> - 1.47.0-2
- Type:bugfix
- ID:NA
- SUG:NA
- DESC:replace the use of strtoul and strol with parse_uint

* Sat Jul 30 2022 tianlijing <tianlijing@kylinos.cn> - 1.47.0-1
- update to 1.47.0

* Sat Mar 19 2022 xihaochen <xihaochen@h-partners.com> - 1.46.0-1
- Type:requirements
- Id:NA
- SUG:NA
- DESC:update nghttp2 to 1.46.0

* Sat Sep 25 2021 sdlzx <hdu_sdlzx@163.com> - 1.45.1-1
- Update to 1.45.1

* Fri Jan 29 2021 xihaochen <xihaochen@huawei.com> - 1.42.0-1
- Type:requirements
- Id:NA
- SUG:NA
- DESC:update nghttp2 to 1.42.0

* Mon Jul 27 2020 cuibaobao <cuibaobao1@huawei.com> - 1.41.0-2
- Type:fix in files
- ID:NA
- SUG:NA
- DESC:fix in files

* Thu Jul 23 2020 cuibaobao <cuibaobao1@huawei.com> - 1.41.0-1
- Type:update
- ID:NA
- SUG:NA
- DESC:update to 1.41.0

* Tue Dec 24 2019 openEuler Buildteam <buildteam@openeuler.org> - 1.39.2-2
- Type:bugfix
- ID:NA
- SUG:NA
- DESC:bugfix in files

* Tue Sep 10 2019 openEuler Buildteam <buildteam@openeuler.org> - 1.39.2-1
- Package init
