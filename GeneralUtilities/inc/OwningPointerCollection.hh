#ifndef GeneralUtilities_OwningPointerCollection_hh
#define GeneralUtilities_OwningPointerCollection_hh
//
// A class template to take ownership of a collection of bare pointers to
// objects, to provide access to those objects and to delete them when the
// container object goes out of scope.
//
// This is designed to allow complex objects made on the heap to be used
// as transient-only data products.
//
// The original use is for BaBar tracks.
//
// $Id: OwningPointerCollection.hh,v 1.7 2013/03/16 23:31:58 kutschke Exp $
// $Author: kutschke $
// $Date: 2013/03/16 23:31:58 $
//
// Original author Rob Kutschke
//
// Notes:
// 1) I believe it would be wise to make the underlying container:
//      std::vector<boost::right_kind_of_smart_pointer<T>>.
//    scoped_ptr will not work since they are not copyable ( needed for vector ).
//    unique_ptr seems mostly concerned with threads.
//    I tried shared_ptr but I could not make it play nice with genreflex.
//

#include <vector>
#include <stdexcept>

namespace mu2e {

  template<typename T>
  class OwningPointerCollection{

  public:

    typedef typename std::vector<const T*>::const_iterator const_iterator;
    typedef T const* value_type;

    OwningPointerCollection():v_(){}

    // Caller transfers ownership of the pointees to us.
    explicit OwningPointerCollection( std::vector<T*>& v ):
      v_(v.begin(), v.end() ){
    }

    // Caller transfers ownership of the pointees to us.
    explicit OwningPointerCollection( std::vector<T const*>& v ):
      v_(v){
    }

    // We own the pointees so delete them when our destructor is called.
    ~OwningPointerCollection(){
      for( typename std::vector<T const *>::iterator i=v_.begin();
           i!=v_.end(); ++i ){
        delete *i;
      }
    }

#ifndef __GCCXML__

    OwningPointerCollection( OwningPointerCollection && rhs ):
      v_(std::move(rhs.v_)){
      rhs.v_.clear();
    }

    OwningPointerCollection& operator=( OwningPointerCollection && rhs ){
      v_ = std::move(rhs.v_);
      rhs.v_.clear();
      return *this;
    }

    // Not copy-copyable or copy-assignable; this is needed to ensure exactly one delete.
    //OwningPointerCollection( OwningPointerCollection const& ) = delete;
    //OwningPointerCollection& operator=( OwningPointerCollection const& ) = delete;

    // For some reason, something in genreflex requires that there to be a copy c'tor!
    // Until we find and fix that, give it one.

    OwningPointerCollection( OwningPointerCollection const& ){
      throw std::logic_error("Must never call the copy c'tor of OwningPointerCollection.");
    }

    OwningPointerCollection& operator=( OwningPointerCollection const& ){
      throw std::logic_error("Must never call the copy c'tor of OwningPointerCollection.");
    }

#endif /* GCCXML */

    // Caller transfers ownership of the pointee to us.
    void push_back( T* t){
      v_.push_back(t);
    }

    void push_back( T const* t){
      v_.push_back(t);
    }

    typename std::vector<const T*>::const_iterator begin() const{
      return v_.begin();
    }

    typename std::vector<const T*>::const_iterator end() const{
      return v_.end();
    }

    // Possibly needed by producer modules?
    /*
    void pop_back( ){
      delete v_.back();
      v_.pop_back();
    }
    */

    // Needed for event.put().
    void swap( OwningPointerCollection& rhs){
      std::swap( this->v_, rhs.v_);
    }

    // Accessors: this container retains ownership of the pointees.
    size_t   size()                    const { return  v_.size(); }
    T const* operator[](std::size_t i) const { return  v_.at(i);  }
    T const* at        (std::size_t i) const { return  v_.at(i);  }
    T const* get       (std::size_t i) const { return  v_.at(i);  }

    // const access to the underlying container.
    std::vector<T const *> const& getAll(){ return v_; }

  private:

    // Owning pointers to the objects.
    std::vector<const T*> v_;

  };

} // namespace mu2e

#endif /* GeneralUtilities_OwningPointerCollection_hh */
