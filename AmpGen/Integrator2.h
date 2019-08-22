#ifndef AMPGEN_INTEGRATOR2_H
#define AMPGEN_INTEGRATOR2_H 1

#include "AmpGen/Integrator.h"

namespace AmpGen {

  template <size_t NROLL = 10>
    class Integrator2
    {
      private:
        typedef const complex_t& arg;
        typedef std::function<void(arg)> TransferFCN;
        size_t                              m_counter = {0};
        std::array<Integral<arg>, NROLL>    m_integrals;
        EventList*                          m_events  = {nullptr};
        std::vector<std::vector<complex_t>> m_buffer;
        std::vector<double>                 m_weight; 
        std::map<std::string, size_t>       m_index; 
        void integrateBlock()
        {
          real_t re[NROLL] = {0};
          real_t im[NROLL] = {0};
          size_t addr_i[NROLL] = {0};
          size_t addr_j[NROLL] = {0};
          for( size_t roll = 0 ; roll < NROLL; ++roll )
          {
            addr_i[roll] = m_integrals[roll].i;
            addr_j[roll] = m_integrals[roll].j;
          }
          #pragma omp parallel for reduction(+: re, im)
          for ( size_t i = 0; i < m_events->size(); ++i ) {
            for ( size_t roll = 0; roll < NROLL; ++roll ) {
              auto c = m_buffer[addr_i[roll]][i] * std::conj(m_buffer[addr_j[roll]][i]);
              re[roll] += m_weight[i] * std::real(c);
              im[roll] += m_weight[i] * std::imag(c);
            }
          }
          real_t nv = m_events->norm();
          for ( size_t j = 0; j < m_counter; ++j )
            m_integrals[j].transfer( complex_t( re[j], im[j] ) / nv );
        }

      public:
        Integrator2( EventList* events = nullptr ) : m_events( events ){
          if( m_events != nullptr ){
            m_weight.resize( m_events->size() );
            for( size_t i = 0 ; i < m_events->size(); ++i ) 
              m_weight[i] = m_events->at(i).weight() / m_events->at(i).genPdf();
          }
        }

        double sampleNorm()             { return m_events->norm(); } 
        bool isReady()            const { return m_events != nullptr; }
        EventList& events()             { return *m_events; } 
        const EventList& events() const { return *m_events; } 
        void queueIntegral(const size_t& c1, 
            const size_t& c2, 
            const size_t& i, 
            const size_t& j, 
            Bilinears* out, 
            const bool& sim = true)
        {
          if( ! out->workToDo(i,j) )return;
          if( sim ) 
            addIntegralKeyed( c1, c2, [out,i,j]( arg& val ){ 
                out->set(i,j,val);
                if( i != j ) out->set(j,i, std::conj(val) ); } );
          else 
            addIntegralKeyed( c1, c2, [out,i,j]( arg& val ){ out->set(i,j,val); } );
        }
        void addIntegralKeyed( const size_t& c1, const size_t& c2, const TransferFCN& tFunc )
        {
          m_integrals[m_counter++] = Integral<arg>(c1,c2,tFunc);
          if ( m_counter == NROLL ){ integrateBlock(), m_counter = 0; }
        }
        void queueIntegral(const size_t& i, const size_t& j, complex_t* result){
          addIntegralKeyed(i, j, [result](arg& val){ *result = val ; } ); 
        }
        void flush()
        {
          if ( m_counter == 0 ) return;
          integrateBlock();
          m_counter =0;
        }
        template <class EXPRESSION>
        size_t getCacheIndex(const EXPRESSION& expression) const {
          return m_index.find(expression.name())->second;
        }
        template <class EXPRESSION>
          void prepareExpression( const EXPRESSION& expression, const size_t& size_of = 0 )
          {
            if( m_events == nullptr ) return; 
            size_t expression_size = size_of == 0 ? expression.returnTypeSize() / sizeof(complex_t) : size_of;
            auto it = m_index.find( expression.name() );
            auto index = 0;
            if( it == m_index.end() )
            {
              index = m_buffer.size();
              m_index[ expression.name() ] = index;
              m_buffer.resize(index+expression_size);
              for(size_t j = 0 ; j != expression_size; ++j )
                m_buffer[index+j].resize( m_events->size() );
            }
            else index = it->second; 
            #ifdef _OPENMP
            #pragma omp parallel for
            #endif
            for ( size_t i = 0; i < m_events->size(); ++i ){
              auto v = expression(m_events->at(i).address());
              setBuffer( &(m_buffer[index][i]), v, expression_size );
            }
          }
        void setBuffer( complex_t* pos, const complex_t& value, const size_t& size )
        {
          *pos = value;
        }
        void setBuffer( complex_t* pos, const std::vector<complex_t>& value, const size_t& size)
        {
          memcpy( pos, &(value[0]), size * sizeof(complex_t) );
        }
    };
}
#endif
