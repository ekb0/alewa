# alewa
A static web server written in ✨ modern c++ ✨

### Alewa Hungarian Convention

This project uses a mild form of hungarian convention. We do not prefix all
pointers with a 'p'.

Instead:

* If a pointer can be null, prefix it with a 'p' and guard every dereference
  directly or indirectly.  
  Also:
    * always initialize such pointers explicitly with a nullptr
    * prefer descriptive names over the prefix. eg;  
      `user* p_user` if used as a loop variable should be `user* it`  
      `user** pp_user;` becomes `user** p_user_list;` or `user** p_user_arr;`

* If a pointer should always point to a valid object, the 'p' should be
  omitted.  
  Also:
    * consider using a reference instead, unless the pointer points to some
      object allocated on the heap or needs to be passed to a pure C function
    * never initialize such pointers with a nullptr
    * do not reassign such pointers, unless when transferring ownership
    * use asserts to verify they are not being reassigned to a nullptr
