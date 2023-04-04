Name:           spglib
Summary:        C library for finding and handling crystal symmetries
Version:        0.0.0
Release:        %{autorelease}
License:        BSD
URL:            https://%{name}.readthedocs.io/

Source0:        https://github.com/spglib/%{name}/archive/refs/tags/v%{version}.tar.gz
Source1:        %{name}.rpmlintrc

BuildRequires:  ninja-build
BuildRequires:  cmake
BuildRequires:  gcc
BuildRequires:  gcc-c++
BuildRequires:  gcc-fortran
BuildRequires:  gtest-devel
BuildRequires:  python3-devel

%description
C library for finding and handling crystal symmetries.

%package        devel
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    devel
This package contains libraries and header files for developing
applications that use %{name}.

%package        fortran
Summary:        Runtime files for %{name} Fortran bindings
Requires:       %{name} = %{version}-%{release}
Requires:       gcc-gfortran%{_isa}

%description    fortran
This package contains runtime files to run applications that were built
using %{name}'s Fortran bindings.

%package        fortran-devel
Summary:        Development files for %{name} with Fortran bindings
Requires:       %{name}-fortran%{?_isa} = %{version}-%{release}
Requires:       %{name}-devel = %{version}-%{release}

%description    fortran-devel
This package contains Fortran module and header files for developing
Fortran applications that use %{name}.

%package -n     python3-%{name}
Summary:        Python3 library of %{name}
Requires:       %{name}

%description -n python3-%{name}
This package contains the libraries to
develop applications with %{name} Python3 bindings.

%prep
%autosetup -n %{name}-%{version}

%generate_buildrequires
%pyproject_buildrequires -x test

%build
%cmake \
    -DSPGLIB_SHARED_LIBS=ON \
    -DSPGLIB_WITH_Fortran=ON \
    -DSPGLIB_WITH_Python=OFF \
    -DSPGLIB_WITH_TESTS=ON

%cmake_build
%pyproject_wheel

%install
%cmake_install

%pyproject_install
%pyproject_save_files %{name}

%check
%ctest
%pytest -v

%files
%doc README.md
%license COPYING
%{_libdir}/libsymspg.so.*

%files fortran
%{_libdir}/libspglib_f08.so.*

%files devel
%{_libdir}/libsymspg.so
%{_includedir}/spglib.h
# Note: Different case-sensitivity comapred to %%{name}
%{_libdir}/cmake/Spglib/SpglibConfig.cmake
%{_libdir}/cmake/Spglib/SpglibConfigVersion.cmake
%{_libdir}/cmake/Spglib/SpglibTargets_shared.cmake
%{_libdir}/cmake/Spglib/SpglibTargets_shared-release.cmake
%{_libdir}/cmake/Spglib/PackageCompsHelper.cmake
%{_libdir}/pkgconfig/spglib.pc

%files fortran-devel
%{_libdir}/libspglib_f08.so
%{_includedir}/spglib_f08.F90
%{_includedir}/spglib_f08.mod
%{_libdir}/pkgconfig/spglib_f08.pc
%{_libdir}/cmake/Spglib/SpglibTargets_fortran_shared.cmake
%{_libdir}/cmake/Spglib/SpglibTargets_fortran_shared-release.cmake

%files -n python3-%{name}
%{python3_sitearch}/%{name}/
%exclude %{python3_sitearch}/%{name}/libsymspg.so*
%exclude %{python3_sitearch}/%{name}/spglib.h
%{python3_sitearch}/%{name}-*.dist-info/

%changelog
%autochangelog
