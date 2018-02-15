Name:    hermod
Version: 0.3
Release: 1%{?dist}
Summary: A C++ web application server.
License: Lesser General Public License

%description
A C++ web application server.

%prep

%build
echo "----------- BUILD -------------"

%check

%install
echo "----------- INSTALL -------------"
install -m 755 -d %{buildroot}/%{_sbindir}
cp -a ../../../src/hermod %{buildroot}/%{_sbindir}/hermod
install -m 755 -d %{buildroot}/%{_libdir}/hermod
cp -a ../../../modules/Dummy/dummy.so %{buildroot}/%{_libdir}/hermod
install -m 755 -d %{buildroot}/%{_sysconfdir}/hermod
cp -a config/main.cfg %{buildroot}/%{_sysconfdir}/hermod

%files
%{_sbindir}/hermod
%{_sysconfdir}/hermod/main.cfg
%{_libdir}/hermod/dummy.so

%changelog
