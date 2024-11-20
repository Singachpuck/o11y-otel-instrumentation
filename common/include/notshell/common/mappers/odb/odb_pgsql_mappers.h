#pragma once

#include <odb/pgsql/traits.hxx>
#include <odb/pgsql/details/endian-traits.hxx>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "notshell/common/mappers/object_mappers.h"

namespace commons = dochkas::notshell::common;

namespace odb
{
  namespace pgsql
  {
    template <>
    class value_traits<boost::posix_time::ptime, id_timestamp>
    {
    public:
      typedef boost::posix_time::ptime value_type;
      typedef boost::posix_time::ptime query_type;
      typedef long long image_type;

      static void
      set_value (value_type& v, const image_type& i, bool is_null)
      {
        if (is_null)
        {
          v = boost::posix_time::ptime(boost::gregorian::date(1970,1,1));
          return;
        }

        commons::convert(i, v);
      }

      static void
      set_image (image_type& i, bool& is_null, const value_type& v)
      {
        is_null = false;

        commons::convert(v, i);
      }
    };
  }
}