%define LIBNAME fmidb

Summary: fmidb library
Name: lib%{LIBNAME}
Version: 18.7.30
Release: 1.el7.fmi
License: MIT
Group: Development/Tools
URL: http://www.fmi.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)
Provides: lib%{LIBNAME}.so
BuildRequires: oracle-instantclient-devel >= 11.2.0.3.0
BuildRequires: unixODBC-devel
BuildRequires: boost-devel >= 1.66
BuildRequires: libpqxx-devel
Requires: oracle-instantclient-basic >= 11.2.0.3.0

%description
FMI database library

%package devel
Summary: development package
Group: Development/Tools

%description devel
Headers and static libraries for fmidb

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n "%{LIBNAME}" 

%build
make %{_smp_mflags} 

%install
rm -rf $RPM_BUILD_ROOT
make install libdir=$RPM_BUILD_ROOT/%{_libdir} includedir=$RPM_BUILD_ROOT/%{_includedir}

%post
umask 007
/sbin/ldconfig > /dev/null 2>&1

%postun
umask 007
/sbin/ldconfig > /dev/null 2>&1

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,0644)
%{_libdir}/lib%{LIBNAME}.so

%files devel
%defattr(-,root,root,0644)
%{_libdir}/lib%{LIBNAME}.a
%{_includedir}/*.h

%changelog
* Mon Jul 30 2018 Mikko Partio <mikko.partio@fmi.fi> - 18.7.30-1.fmi
- Bugfix for rgg query
* Wed Apr 10 2018 Mikko Partio <mikko.partio@fmi.fi> - 18.4.10-1.fmi
- New boost
* Mon Feb 18 2018 Mikko Partio <mikko.partio@fmi.fi> - 18.2.19-1.fmi
- Bugfix for GetLatestTime()
* Wed Feb 14 2018 Mikko Partio <mikko.partio@fmi.fi> - 18.2.14-1.fmi
- Add function to get newbase name from univ id
* Thu Feb  8 2018 Mikko Partio <mikko.partio@fmi.fi> - 18.2.8-1.fmi
- radon GetGeometryDefinition change
* Wed Jan 10 2018 Mikko Partio <mikko.partio@fmi.fi> - 18.1.10-1.fmi
- Improved station information fetching
* Mon Sep 18 2017 Mikko Partio <mikko.partio@fmi.fi> - 17.9.18-1.fmi
- Add function to warm grib1 parameter cache
* Fri Sep 15 2017 Mikko Partio <mikko.partio@fmi.fi> - 17.9.15-1.fmi
- Add function to fetch target table information
* Thu Aug 10 2017 Mikko Partio <mikko.partio@fmi.fi> - 17.8.10-1.fmi
- Bugfixes and optimizations
* Thu Jun  8 2017 Mikko Partio <mikko.partio@fmi.fi> - 17.6.8-1.fmi
- Fetch latest time for previ producers
* Fri May 26 2017 Mikko Partio <mikko.partio@fmi.fi> - 17.5.26-1.fmi
- Fetch station probability limits from database
* Tue Apr 11 2017 Mikko Partio <mikko.partio@fmi.fi> - 17.4.11-1.fmi
- Bugfix for precision setting
* Thu Apr  6 2017 Mikko Partio <mikko.partio@fmi.fi> - 17.4.6-1.fmi
- User radon_client for radon
- Add function to get decimal precision for a parameter
* Wed Feb 15 2017 Mikko Partio <mikko.partio@fmi.fi> - 17.2.15-1.fmi
- Cleanup and username/password fixes
* Fri Feb  3 2017 Mikko Partio <mikko.partio@fmi.fi> - 17.2.3-1.fmi
- Base and scale fetched from Radon
* Tue Jan 10 2017 Mikko Partio <mikko.partio@fmi.fi> - 17.1.10-1.fmi
- Ship sounding fixes
* Fri Dec  9 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.12.9-1.fmi
- Radon parameter query with level information
* Wed Dec  7 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.12.7-1.fmi
- Fix for CLDB query
* Fri Nov 25 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.11.25-1.fmi
- Removing an invalid reference to table as_grid
* Tue Nov 22 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.11.22-1.fmi
- Latest time fetched not using record_count (as_grid)
* Tue Nov 15 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.11.15-1.fmi
- More station information fetched from neons
* Tue Nov  8 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.11.8-2.fmi
- Support table param_grib2_template
* Tue Nov  8 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.11.8-1.fmi
- Change to level parameter mapping
* Wed Nov  2 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.11.2-1.fmi
- New release
* Wed Oct 26 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.10.26-1.fmi
- New release
* Tue Oct 25 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.10.25-1.fmi
- Using min/max analysis times for radon
* Mon Sep 26 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.9.26-1.fmi
- Lambert support
* Thu Sep  8 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.9.8-1.fmi
- New release to support bdmlegacy
* Tue Aug 30 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.8.30-1.fmi
- New release
* Mon Aug 29 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.8.29-1.fmi
- New release
* Tue Aug 23 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.8.23-2.fmi
- Bugfix release
* Tue Aug 23 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.8.23-1.fmi
- New release
* Tue Jul 12 2016 Mikko Vanhatalo <mikko.vanhatalo@fmi.fi> - 16.7.12-1.fmi
- get producer meta data for radon
- CLDB station queries for producer 20016 now use WMO groups 125 and 127
* Thu Jun  9 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.6.9-1.fmi
- Kiitotie stations for 20013
- New function to radondb
* Mon Jun  6 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.6.6-1.fmi
- type_id support extension
* Thu May 26 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.5.26-1.fmi
- producer_type table in radon database
* Fri Feb 12 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.2.12-1.fmi
- Change signature of GetLevelTransform() so that more information can be returned
* Mon Feb  8 2016 Mikko Partio <mikko.partio@fmi.fi> - 16.2.8-1.fmi
- Bugfix for radon level xref
* Mon Nov 16 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.11.16-1.fmi
- Change producer 20014 to cover all external road weather stations (Sweden,Norway,Estonia)
* Wed Nov 11 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.11.11-1.fmi
- Re-throw pqx::unique_violation
* Thu Nov  5 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.11.5-1.fmi
- Correct rounding of coordinates when fetching geom info (case icemap2)
* Thu Oct  8 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.10.9-1.fmi
- Add geometry name to return value of GetGridGeoms()
* Thu Oct  8 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.10.8-1.fmi
- Radon bugfix
* Tue Sep 29 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.9.29-1.fmi
- New function to get level transforms
* Mon Sep 28 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.9.28-1.fmi
- Minor bugfixes
* Tue Sep  1 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.9.1-1.fmi
- Add ClassName() function
* Mon Aug 10 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.8.10-2.fmi
- Fix to transaction handling
* Mon Aug 10 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.8.10-1.fmi
- Additions to NFmiPostgreSQL
* Wed Jun 24 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.6.24-2.fmi
- Fix to error handling
* Wed Jun 24 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.6.24-1.fmi
- New database type NFmiPostgreSQL that uses native driver
* Fri Apr 24 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.4.24-1.fmi
- Optimizing core query at NFmiNeonsDB that fetches parameter metadata
* Thu Apr 16 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.4.16-1.fmi
- New radon functions
* Tue Mar 17 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.3.17-1.fmi
- New function to get dataset information
* Tue Mar 10 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.3.10-1.fmi
- New function to get geometry definition from area information
* Fri Feb  6 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.2.6-1.fmi
- latest time modifications
* Thu Jan 22 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.1.22-1.fmi
- Radon changes
* Wed Jan  7 2015 Mikko Partio <mikko.partio@fmi.fi> - 15.1.7-1.fmi
- Lots of radon changes
* Mon Dec 29 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.12.29-1.fmi
- New release
* Thu Dec 18 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.12.18-1.fmi
- Removing oracle-specific stuff from NFmiODBC
* Wed Dec 17 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.12.17-1.fmi
- Radon additions
* Thu Oct 30 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.10.30-2.fmi
- Semicolon-fix
* Thu Oct 30 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.10.30-1.fmi
- Level support for grib1 parameter definition
* Tue Oct 28 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.10.28-1.fmi
- Changes wrt radon
* Mon Oct 20 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.10.20-1.fmi
- neon2 --> radon
* Thu Sep  4 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.9.4-1.fmi
- Fix grib2 parameter metadata fetching
* Mon Sep  1 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.9.1-1.fmi
- Adding functions to NFmiNeon2DB
* Fri Aug 22 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.8.22-1.fmi
- Adding functions to NFmiNeon2DB
* Tue Aug 19 2014 Andreas Tack <andreas.tack@fmi.fi> - 14.8.19-2.fmi
- Change some exit statements to throw error instead
* Tue Aug 19 2014 Andreas Tack <andreas.tack@fmi.fi> - 14.8.19-1.fmi
- Neon2 support
* Mon Aug  4 2014 Mikko Partio <mikko.partio@fmi.fi> - 14.8.4-1.fmi
- Smaller fixes
* Tue Nov 19 2013 Mikko Partio <mikko.partio@fmi.fi> - 13.11.19-1.fmi
- Supporting timeRangeIndicator in parameter metadata retrieval - API change!
* Wed Nov 13 2013 Mikko Partio <mikko.partio@fmi.fi> - 13.11.13-1.fmi
- Bugfix release
* Tue Oct  8 2013 Mikko Partio <mikko.partio@fmi.fi> - 13.10.8-1.fmi
- Separating devel-package
* Wed Oct  2 2013 Mikko Partio <mikko.partio@fmi.fi> - 13.10.2-1.fmi
- Add SONAME
* Tue Oct  1 2013 Mikko Partio <mikko.partio@fmi.fi> - 13.10.1-1.fmi
- Initial build
