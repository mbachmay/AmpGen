#ifndef AMPGEN_EVENT_H
#define AMPGEN_EVENT_H

#include <vector>
#include <complex>
#include <cstring>
#include <cstddef>
#include "AmpGen/Types.h"

namespace AmpGen { 
  class Event {
    private:
      std::vector<real_t>    m_event; 
      std::vector<complex_t> m_cache;
      real_t                 m_genPdf;
      real_t                 m_weight; 

      inline real_t get(const size_t& index ) const { return m_event[index]; };
    public:

      Event( const size_t& N, const size_t& cacheSize=0 ) : 
        m_event(N), 
        m_cache(cacheSize), 
        m_genPdf(1),
        m_weight(1) {}

      Event( const real_t* data, const size_t& N, const size_t& cacheSize=0) :
        m_event(data, data+N),
        m_cache(cacheSize),
        m_genPdf(1),
        m_weight(1) {}

      void set( const size_t& i, const std::vector<real_t>& p ){
        for( size_t j = 0 ; j < 4; ++j) m_event[4*i + j ] = p[j];
      }
      void set( const size_t& i, const real_t* p ){
        for( size_t j = 0 ; j < 4; ++j) m_event[4*i + j ] = p[j];
      }
      void set( const real_t* evt ){
        for( size_t i = 0 ; i < m_event.size(); ++i ) m_event[i] = *(evt + i );
      }
      void set( const size_t& i, const real_t& p ){ m_event[i] = p ; } 
      void swap( const unsigned int& i , const unsigned int& j ){
        double tmp[4];
        std::memmove( tmp, &m_event[4*j], sizeof(tmp)); 
        std::memmove( &m_event[4*j], &m_event[4*i],sizeof(tmp));
        std::memmove( &m_event[4*i], &tmp,sizeof(tmp));
      }
      void invertParity( unsigned int nParticles=0){
        if( nParticles == 0 ) nParticles = size() / 4; 
        for( unsigned int i = 0 ; i < nParticles  ; ++i ){
          m_event[4*i + 0 ] = -m_event[4*i+0];
          m_event[4*i + 1 ] = -m_event[4*i+1];
          m_event[4*i + 2 ] = -m_event[4*i+2];
        }
      }

      void setCache(const complex_t& value, const size_t& pos){ m_cache[pos] = value; }
      template < size_t N > void setCache( const std::array<complex_t,N>& value, const size_t& pos )
      {
        std::memmove( m_cache.data() + pos, value.data(), sizeof(std::array<complex_t,N>) );
      }
      void setCache( const std::vector<complex_t>& value, const size_t& pos )
      {
        std::memmove( m_cache.data() + pos, value.data(), sizeof(complex_t) * value.size() );
      }

      void resizeCache( const unsigned int& new_size ){ m_cache.resize(new_size); }
      
      size_t   size()                                         const { return m_event.size(); } 

      real_t* pWeight()                                             { return &(m_weight); }
      real_t* pGenPdf()                                             { return &m_genPdf; }
      const real_t* address(const unsigned int ref=0 )        const { return &(m_event[ref]); }
      real_t*       address(const unsigned int& ref=0)              { return &(m_event[ref]); }

      unsigned int cacheSize()                                const { return m_cache.size(); } 
      real_t weight()                                         const { return m_weight; } 
      real_t genPdf()                                         const { return m_genPdf; }
      real_t  operator[](const unsigned int& i)               const { return m_event[i]; }
      real_t& operator[](const unsigned int& i)                     { return m_event[i]; }
      operator const real_t*()                                const { return &(m_event[0]); }
      operator       real_t*()                                      { return &(m_event[0]); }

      const complex_t& getCache(const unsigned int& pos)      const { return m_cache[pos]; }
      const complex_t* getCachePtr(const unsigned int& pos=0) const { return &(m_cache[0]) + pos; }

      void setWeight( const real_t& weight ){ m_weight = weight ; } 
      void setGenPdf( const real_t& genPdf ){ m_genPdf = genPdf ; } 
      void extendEvent(const real_t& value) { m_event.push_back( value ); } 

      void print()      const;
      void printCache() const;

      real_t s( const size_t& index) const ;  
      real_t s( const size_t& index1, const size_t& index2 ) const ;
      real_t s( const size_t& index1, const size_t& index2, const size_t& index3 ) const;
      real_t s( const std::vector<size_t>& indices ) const ;
  };
}

#endif 
