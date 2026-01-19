# Python-DAP API Index

This document lists Python API symbols defined in python-dap source files.
Each entry includes the file that defines the symbol.

Notes:
- The Python lists include all class methods and functions found in source files, including private helpers.
- C extension entries are extracted from PyMethodDef arrays in C sources.

## Python modules (dap.*)

### dap
File: python-dap/src/dap/__init__.py

Functions:
- get_dap (python-dap/src/dap/__init__.py)
- init_dap (python-dap/src/dap/__init__.py)
- deinit_dap (python-dap/src/dap/__init__.py)
- dap_status (python-dap/src/dap/__init__.py)
- log_it (python-dap/src/dap/__init__.py)

Classes:
- Dap (python-dap/src/dap/__init__.py)
  - Dap.__init__ (python-dap/src/dap/__init__.py)
  - Dap.__new__ (python-dap/src/dap/__init__.py)
  - Dap.init (python-dap/src/dap/__init__.py)
  - Dap.deinit (python-dap/src/dap/__init__.py)
  - Dap._cleanup_on_failure (python-dap/src/dap/__init__.py)
  - Dap.status (python-dap/src/dap/__init__.py)
  - Dap.type (python-dap/src/dap/__init__.py)
  - Dap.log (python-dap/src/dap/__init__.py)
  - Dap.time (python-dap/src/dap/__init__.py)
  - Dap.system (python-dap/src/dap/__init__.py)
  - Dap.is_initialized (python-dap/src/dap/__init__.py)
  - Dap.mark_global_session_active (python-dap/src/dap/__init__.py)
  - Dap.mark_global_session_inactive (python-dap/src/dap/__init__.py)
  - Dap.__enter__ (python-dap/src/dap/__init__.py)
  - Dap.__exit__ (python-dap/src/dap/__init__.py)
  - Dap.__repr__ (python-dap/src/dap/__init__.py)
  - Dap.global_db (python-dap/src/dap/__init__.py)
- DapCoreError (python-dap/src/dap/__init__.py)

### dap.common
File: python-dap/src/dap/common/__init__.py

(No Python-level functions or classes defined)

### dap.common.exceptions
File: python-dap/src/dap/common/exceptions.py

Functions:
- format_exception_context (python-dap/src/dap/common/exceptions.py)
- create_exception_from_error_code (python-dap/src/dap/common/exceptions.py)

Classes:
- DapConfigError (python-dap/src/dap/common/exceptions.py)
  - DapConfigError.__init__ (python-dap/src/dap/common/exceptions.py)
- DapCoreError (python-dap/src/dap/common/exceptions.py)
  - DapCoreError.__init__ (python-dap/src/dap/common/exceptions.py)
- DapCryptoError (python-dap/src/dap/common/exceptions.py)
  - DapCryptoError.__init__ (python-dap/src/dap/common/exceptions.py)
- DapEventError (python-dap/src/dap/common/exceptions.py)
  - DapEventError.__init__ (python-dap/src/dap/common/exceptions.py)
- DapException (python-dap/src/dap/common/exceptions.py)
  - DapException.__init__ (python-dap/src/dap/common/exceptions.py)
  - DapException.add_context (python-dap/src/dap/common/exceptions.py)
  - DapException.add_suggestion (python-dap/src/dap/common/exceptions.py)
  - DapException.get_full_context (python-dap/src/dap/common/exceptions.py)
  - DapException.__str__ (python-dap/src/dap/common/exceptions.py)
  - DapException.__repr__ (python-dap/src/dap/common/exceptions.py)
- DapInitializationError (python-dap/src/dap/common/exceptions.py)
  - DapInitializationError.__init__ (python-dap/src/dap/common/exceptions.py)
- DapLogError (python-dap/src/dap/common/exceptions.py)
  - DapLogError.__init__ (python-dap/src/dap/common/exceptions.py)
- DapNetworkError (python-dap/src/dap/common/exceptions.py)
  - DapNetworkError.__init__ (python-dap/src/dap/common/exceptions.py)
- DapSystemError (python-dap/src/dap/common/exceptions.py)
  - DapSystemError.__init__ (python-dap/src/dap/common/exceptions.py)
- DapTimeError (python-dap/src/dap/common/exceptions.py)
  - DapTimeError.__init__ (python-dap/src/dap/common/exceptions.py)
- DapTypeError (python-dap/src/dap/common/exceptions.py)
  - DapTypeError.__init__ (python-dap/src/dap/common/exceptions.py)

### dap.common.types
File: python-dap/src/dap/common/types.py

Functions:
- format_bytes (python-dap/src/dap/common/types.py)

Classes:
- DapType (python-dap/src/dap/common/types.py)
  - DapType.__init__ (python-dap/src/dap/common/types.py)
  - DapType.format_bytes (python-dap/src/dap/common/types.py)
  - DapType.__repr__ (python-dap/src/dap/common/types.py)
- DapTypeError (python-dap/src/dap/common/types.py)

### dap.config
File: python-dap/src/dap/config/__init__.py

(No Python-level functions or classes defined)

### dap.config.config
File: python-dap/src/dap/config/config.py

Functions:
- get_dap_config (python-dap/src/dap/config/config.py)

Classes:
- DapConfig (python-dap/src/dap/config/config.py)
  - DapConfig.__init__ (python-dap/src/dap/config/config.py)
  - DapConfig.__new__ (python-dap/src/dap/config/config.py)
  - DapConfig.init (python-dap/src/dap/config/config.py)
  - DapConfig.deinit (python-dap/src/dap/config/config.py)
  - DapConfig.get_item_str (python-dap/src/dap/config/config.py)
  - DapConfig.get_item_int (python-dap/src/dap/config/config.py)
  - DapConfig.get_item_bool (python-dap/src/dap/config/config.py)
  - DapConfig.set_item_str (python-dap/src/dap/config/config.py)
  - DapConfig.set_item_int (python-dap/src/dap/config/config.py)
  - DapConfig.set_item_bool (python-dap/src/dap/config/config.py)
  - DapConfig.get_sys_dir (python-dap/src/dap/config/config.py)
  - DapConfig.is_initialized (python-dap/src/dap/config/config.py)
  - DapConfig.config_file (python-dap/src/dap/config/config.py)
  - DapConfig.__enter__ (python-dap/src/dap/config/config.py)
  - DapConfig.__exit__ (python-dap/src/dap/config/config.py)
  - DapConfig.__repr__ (python-dap/src/dap/config/config.py)

### dap.config.file
File: python-dap/src/dap/config/file.py

Classes:
- DapConfigFile (python-dap/src/dap/config/file.py)
  - DapConfigFile.__init__ (python-dap/src/dap/config/file.py)
  - DapConfigFile.open (python-dap/src/dap/config/file.py)
  - DapConfigFile.close (python-dap/src/dap/config/file.py)
  - DapConfigFile.get_item_str (python-dap/src/dap/config/file.py)
  - DapConfigFile.get_item_int (python-dap/src/dap/config/file.py)
  - DapConfigFile.get_item_bool (python-dap/src/dap/config/file.py)
  - DapConfigFile.set_item_str (python-dap/src/dap/config/file.py)
  - DapConfigFile.set_item_int (python-dap/src/dap/config/file.py)
  - DapConfigFile.set_item_bool (python-dap/src/dap/config/file.py)
  - DapConfigFile.__enter__ (python-dap/src/dap/config/file.py)
  - DapConfigFile.__exit__ (python-dap/src/dap/config/file.py)
  - DapConfigFile.__repr__ (python-dap/src/dap/config/file.py)

### dap.crypto
File: python-dap/src/dap/crypto/__init__.py

(No Python-level functions or classes defined)

### dap.crypto.cert
File: python-dap/src/dap/crypto/cert.py

Functions:
- load (python-dap/src/dap/crypto/cert.py)
- find (python-dap/src/dap/crypto/cert.py)

Classes:
- DapCert (python-dap/src/dap/crypto/cert.py)
  - DapCert.__init__ (python-dap/src/dap/crypto/cert.py)
  - DapCert.__del__ (python-dap/src/dap/crypto/cert.py)
  - DapCert.handle (python-dap/src/dap/crypto/cert.py)
  - DapCert.generate (python-dap/src/dap/crypto/cert.py)
  - DapCert.load (python-dap/src/dap/crypto/cert.py)
  - DapCert.save (python-dap/src/dap/crypto/cert.py)
  - DapCert.create (python-dap/src/dap/crypto/cert.py)
  - DapCert.sign (python-dap/src/dap/crypto/cert.py)
  - DapCert.verify (python-dap/src/dap/crypto/cert.py)
  - DapCert.get_private_key (python-dap/src/dap/crypto/cert.py)
  - DapCert.__enter__ (python-dap/src/dap/crypto/cert.py)
  - DapCert.__exit__ (python-dap/src/dap/crypto/cert.py)
- DapCertChain (python-dap/src/dap/crypto/cert.py)
  - DapCertChain.__init__ (python-dap/src/dap/crypto/cert.py)
  - DapCertChain.add_certificate (python-dap/src/dap/crypto/cert.py)
  - DapCertChain.verify_chain (python-dap/src/dap/crypto/cert.py)
  - DapCertChain.__enter__ (python-dap/src/dap/crypto/cert.py)
  - DapCertChain.__exit__ (python-dap/src/dap/crypto/cert.py)
- DapCertError (python-dap/src/dap/crypto/cert.py)
- DapCertStore (python-dap/src/dap/crypto/cert.py)
  - DapCertStore.__init__ (python-dap/src/dap/crypto/cert.py)
  - DapCertStore.add_certificate (python-dap/src/dap/crypto/cert.py)
  - DapCertStore.get_certificate (python-dap/src/dap/crypto/cert.py)
  - DapCertStore.delete_certificate (python-dap/src/dap/crypto/cert.py)
  - DapCertStore.__enter__ (python-dap/src/dap/crypto/cert.py)
  - DapCertStore.__exit__ (python-dap/src/dap/crypto/cert.py)
- DapCertType (python-dap/src/dap/crypto/cert.py)
  - DapCertType.from_key_type (python-dap/src/dap/crypto/cert.py)

### dap.crypto.enc
File: python-dap/src/dap/crypto/enc.py

Functions:
- quick_encrypt (python-dap/src/dap/crypto/enc.py)
- quick_decrypt (python-dap/src/dap/crypto/enc.py)

Classes:
- DapEnc (python-dap/src/dap/crypto/enc.py)
  - DapEnc.__init__ (python-dap/src/dap/crypto/enc.py)
  - DapEnc.encrypt (python-dap/src/dap/crypto/enc.py)
  - DapEnc.decrypt (python-dap/src/dap/crypto/enc.py)
  - DapEnc.key_new_generate (python-dap/src/dap/crypto/enc.py)
  - DapEnc.key_delete (python-dap/src/dap/crypto/enc.py)
  - DapEnc.enc_type (python-dap/src/dap/crypto/enc.py)
  - DapEnc.enc_type (python-dap/src/dap/crypto/enc.py)
  - DapEnc.__repr__ (python-dap/src/dap/crypto/enc.py)
- DapEncError (python-dap/src/dap/crypto/enc.py)
- DapEncType (python-dap/src/dap/crypto/enc.py)

### dap.crypto.hash
File: python-dap/src/dap/crypto/hash.py

Functions:
- quick_hash_fast (python-dap/src/dap/crypto/hash.py)

Classes:
- DapHash (python-dap/src/dap/crypto/hash.py)
  - DapHash.__init__ (python-dap/src/dap/crypto/hash.py)
  - DapHash.handle (python-dap/src/dap/crypto/hash.py)
  - DapHash.__del__ (python-dap/src/dap/crypto/hash.py)
  - DapHash.hash_fast (python-dap/src/dap/crypto/hash.py)
  - DapHash.create (python-dap/src/dap/crypto/hash.py)
  - DapHash.__enter__ (python-dap/src/dap/crypto/hash.py)
  - DapHash.__exit__ (python-dap/src/dap/crypto/hash.py)
- DapHashError (python-dap/src/dap/crypto/hash.py)
- DapHashType (python-dap/src/dap/crypto/hash.py)
  - DapHashType.default (python-dap/src/dap/crypto/hash.py)

### dap.crypto.keys
File: python-dap/src/dap/crypto/keys.py

Functions:
- quick_encrypt (python-dap/src/dap/crypto/keys.py)
- quick_decrypt (python-dap/src/dap/crypto/keys.py)

Classes:
- DapKey (python-dap/src/dap/crypto/keys.py)
  - DapKey.__init__ (python-dap/src/dap/crypto/keys.py)
  - DapKey.__del__ (python-dap/src/dap/crypto/keys.py)
  - DapKey.key_type (python-dap/src/dap/crypto/keys.py)
  - DapKey.handle (python-dap/src/dap/crypto/keys.py)
  - DapKey.sign (python-dap/src/dap/crypto/keys.py)
  - DapKey.verify (python-dap/src/dap/crypto/keys.py)
  - DapKey.encrypt (python-dap/src/dap/crypto/keys.py)
  - DapKey.decrypt (python-dap/src/dap/crypto/keys.py)
  - DapKey.key_new_generate (python-dap/src/dap/crypto/keys.py)
  - DapKey.key_delete (python-dap/src/dap/crypto/keys.py)
  - DapKey.get_public_key (python-dap/src/dap/crypto/keys.py)
  - DapKey.get_private_key (python-dap/src/dap/crypto/keys.py)
  - DapKey.get_id (python-dap/src/dap/crypto/keys.py)
  - DapKey.__enter__ (python-dap/src/dap/crypto/keys.py)
  - DapKey.__exit__ (python-dap/src/dap/crypto/keys.py)
  - DapKey.__repr__ (python-dap/src/dap/crypto/keys.py)
- DapKeyError (python-dap/src/dap/crypto/keys.py)
- DapKeyManager (python-dap/src/dap/crypto/keys.py)
  - DapKeyManager.__init__ (python-dap/src/dap/crypto/keys.py)
  - DapKeyManager.create_key (python-dap/src/dap/crypto/keys.py)
  - DapKeyManager.get_key (python-dap/src/dap/crypto/keys.py)
  - DapKeyManager.delete_key (python-dap/src/dap/crypto/keys.py)
  - DapKeyManager.__enter__ (python-dap/src/dap/crypto/keys.py)
  - DapKeyManager.__exit__ (python-dap/src/dap/crypto/keys.py)
- DapKeyType (python-dap/src/dap/crypto/keys.py)
  - DapKeyType.from_string (python-dap/src/dap/crypto/keys.py)
  - DapKeyType.is_post_quantum (python-dap/src/dap/crypto/keys.py)
  - DapKeyType.is_signature_algorithm (python-dap/src/dap/crypto/keys.py)
  - DapKeyType.is_key_exchange_algorithm (python-dap/src/dap/crypto/keys.py)
  - DapKeyType.is_symmetric_algorithm (python-dap/src/dap/crypto/keys.py)
  - DapKeyType.is_deprecated (python-dap/src/dap/crypto/keys.py)
  - DapKeyType.is_quantum_vulnerable (python-dap/src/dap/crypto/keys.py)

### dap.crypto.sign
File: python-dap/src/dap/crypto/sign.py

Functions:
- get_recommended_signature_types (python-dap/src/dap/crypto/sign.py)
- get_deprecated_signature_types (python-dap/src/dap/crypto/sign.py)
- get_quantum_vulnerable_signature_types (python-dap/src/dap/crypto/sign.py)
- get_legacy_deprecated_signature_types (python-dap/src/dap/crypto/sign.py)
- check_signature_compatibility (python-dap/src/dap/crypto/sign.py)

Classes:
- DapSign (python-dap/src/dap/crypto/sign.py)
  - DapSign.sign (python-dap/src/dap/crypto/sign.py)
  - DapSign._create_cert_signature_static (python-dap/src/dap/crypto/sign.py)
  - DapSign._key_type_to_sign_type (python-dap/src/dap/crypto/sign.py)
  - DapSign._create_single_signature_static (python-dap/src/dap/crypto/sign.py)
  - DapSign._create_multi_signature_static (python-dap/src/dap/crypto/sign.py)
  - DapSign._create_composite_static (python-dap/src/dap/crypto/sign.py)
  - DapSign._create_aggregated_static (python-dap/src/dap/crypto/sign.py)
  - DapSign.__init__ (python-dap/src/dap/crypto/sign.py)
  - DapSign.handle (python-dap/src/dap/crypto/sign.py)
  - DapSign.sign_type (python-dap/src/dap/crypto/sign.py)
  - DapSign.keys (python-dap/src/dap/crypto/sign.py)
  - DapSign.metadata (python-dap/src/dap/crypto/sign.py)
  - DapSign.as_bytes (python-dap/src/dap/crypto/sign.py)
  - DapSign.__del__ (python-dap/src/dap/crypto/sign.py)
  - DapSign.check_capability (python-dap/src/dap/crypto/sign.py)
  - DapSign.is_quantum_secure (python-dap/src/dap/crypto/sign.py)
  - DapSign.is_deprecated (python-dap/src/dap/crypto/sign.py)
  - DapSign.is_quantum_vulnerable (python-dap/src/dap/crypto/sign.py)
  - DapSign.supports_multi_signature (python-dap/src/dap/crypto/sign.py)
  - DapSign.supports_aggregation (python-dap/src/dap/crypto/sign.py)
  - DapSign.sign_add (python-dap/src/dap/crypto/sign.py)
  - DapSign.verify (python-dap/src/dap/crypto/sign.py)
  - DapSign.get_size (python-dap/src/dap/crypto/sign.py)
  - DapSign.verify_size (python-dap/src/dap/crypto/sign.py)
  - DapSign.is_aggregated (python-dap/src/dap/crypto/sign.py)
  - DapSign.type_supports_aggregation (python-dap/src/dap/crypto/sign.py)
  - DapSign.get_recommended_types_string (python-dap/src/dap/crypto/sign.py)
  - DapSign.convert_key_type_to_sign_type (python-dap/src/dap/crypto/sign.py)
  - DapSign.convert_sign_type_to_key_type (python-dap/src/dap/crypto/sign.py)
- DapSignError (python-dap/src/dap/crypto/sign.py)
- DapSignMetadata (python-dap/src/dap/crypto/sign.py)
  - DapSignMetadata.get_metadata (python-dap/src/dap/crypto/sign.py)
  - DapSignMetadata.supports_multi_signature (python-dap/src/dap/crypto/sign.py)
  - DapSignMetadata.is_quantum_vulnerable (python-dap/src/dap/crypto/sign.py)
  - DapSignMetadata.is_deprecated (python-dap/src/dap/crypto/sign.py)
- DapSignType (python-dap/src/dap/crypto/sign.py)

### dap.global_db
File: python-dap/src/dap/global_db/__init__.py

(No Python-level functions or classes defined)

### dap.global_db.gdb
File: python-dap/src/dap/global_db/gdb.py

Classes:
- GlobalDB (python-dap/src/dap/global_db/gdb.py)
  - GlobalDB.__new__ (python-dap/src/dap/global_db/gdb.py)
  - GlobalDB.__init__ (python-dap/src/dap/global_db/gdb.py)
  - GlobalDB.init (python-dap/src/dap/global_db/gdb.py)
  - GlobalDB.deinit (python-dap/src/dap/global_db/gdb.py)
  - GlobalDB.set (python-dap/src/dap/global_db/gdb.py)
  - GlobalDB.get (python-dap/src/dap/global_db/gdb.py)
  - GlobalDB.delete (python-dap/src/dap/global_db/gdb.py)
- GlobalDbError (python-dap/src/dap/global_db/gdb.py)

### dap.global_db.global_db
File: python-dap/src/dap/global_db/global_db.py

Classes:
- GlobalDB (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.__new__ (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.__init__ (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB._init (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.deinit (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.set (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.get (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.delete (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.get_keys (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.create (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.read (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.update (python-dap/src/dap/global_db/global_db.py)
  - GlobalDB.exists (python-dap/src/dap/global_db/global_db.py)
- GlobalDBError (python-dap/src/dap/global_db/global_db.py)

### dap.global_db.global_db_cluster
File: python-dap/src/dap/global_db/global_db_cluster.py

Classes:
- GlobalDBCluster (python-dap/src/dap/global_db/global_db_cluster.py)
  - GlobalDBCluster.create (python-dap/src/dap/global_db/global_db_cluster.py)
  - GlobalDBCluster.__init__ (python-dap/src/dap/global_db/global_db_cluster.py)
  - GlobalDBCluster.delete (python-dap/src/dap/global_db/global_db_cluster.py)
  - GlobalDBCluster.member_add (python-dap/src/dap/global_db/global_db_cluster.py)
  - GlobalDBCluster.member_delete (python-dap/src/dap/global_db/global_db_cluster.py)
  - GlobalDBCluster.notify (python-dap/src/dap/global_db/global_db_cluster.py)
  - GlobalDBCluster.notify_add (python-dap/src/dap/global_db/global_db_cluster.py)
  - GlobalDBCluster.broadcast (python-dap/src/dap/global_db/global_db_cluster.py)

### dap.io
File: python-dap/src/dap/io/__init__.py

(No Python-level functions or classes defined)

### dap.io.context
File: python-dap/src/dap/io/context.py

Classes:
- DapContext (python-dap/src/dap/io/context.py)
  - DapContext.__init__ (python-dap/src/dap/io/context.py)
  - DapContext.__repr__ (python-dap/src/dap/io/context.py)
- DapContextError (python-dap/src/dap/io/context.py)

### dap.io.esocket
File: python-dap/src/dap/io/esocket.py

Functions:
- get_dap_events (python-dap/src/dap/io/esocket.py)

Classes:
- DapEventType (python-dap/src/dap/io/esocket.py)
- DapEvents (python-dap/src/dap/io/esocket.py)
  - DapEvents.__init__ (python-dap/src/dap/io/esocket.py)
  - DapEvents.__new__ (python-dap/src/dap/io/esocket.py)
  - DapEvents.init (python-dap/src/dap/io/esocket.py)
  - DapEvents.deinit (python-dap/src/dap/io/esocket.py)
  - DapEvents.start (python-dap/src/dap/io/esocket.py)
  - DapEvents.stop (python-dap/src/dap/io/esocket.py)
  - DapEvents.create_socket (python-dap/src/dap/io/esocket.py)
  - DapEvents.delete_socket (python-dap/src/dap/io/esocket.py)
  - DapEvents.assign_socket_to_worker (python-dap/src/dap/io/esocket.py)
  - DapEvents.add_event_handler (python-dap/src/dap/io/esocket.py)
  - DapEvents.remove_event_handler (python-dap/src/dap/io/esocket.py)
  - DapEvents.emit (python-dap/src/dap/io/esocket.py)
  - DapEvents.subscribe (python-dap/src/dap/io/esocket.py)
  - DapEvents.is_initialized (python-dap/src/dap/io/esocket.py)
  - DapEvents.is_started (python-dap/src/dap/io/esocket.py)
  - DapEvents.workers_count (python-dap/src/dap/io/esocket.py)
  - DapEvents.queue_size (python-dap/src/dap/io/esocket.py)
  - DapEvents.__enter__ (python-dap/src/dap/io/esocket.py)
  - DapEvents.__exit__ (python-dap/src/dap/io/esocket.py)

### dap.io.proc_thread
File: python-dap/src/dap/io/proc_thread.py

Classes:
- DapProcThread (python-dap/src/dap/io/proc_thread.py)
  - DapProcThread.__init__ (python-dap/src/dap/io/proc_thread.py)
  - DapProcThread.__repr__ (python-dap/src/dap/io/proc_thread.py)
- DapProcThreadError (python-dap/src/dap/io/proc_thread.py)

### dap.io.worker
File: python-dap/src/dap/io/worker.py

Classes:
- DapWorker (python-dap/src/dap/io/worker.py)
  - DapWorker.__init__ (python-dap/src/dap/io/worker.py)
  - DapWorker.__repr__ (python-dap/src/dap/io/worker.py)
- DapWorkerError (python-dap/src/dap/io/worker.py)

### dap.json
File: python-dap/src/dap/json/__init__.py

(No Python-level functions or classes defined)

### dap.json.json_utils
File: python-dap/src/dap/json/json_utils.py

Functions:
- json_dump (python-dap/src/dap/json/json_utils.py)
- json_load (python-dap/src/dap/json/json_utils.py)
- json_load_file (python-dap/src/dap/json/json_utils.py)
- json_save_file (python-dap/src/dap/json/json_utils.py)
- json_pretty_print (python-dap/src/dap/json/json_utils.py)
- json_validate (python-dap/src/dap/json/json_utils.py)
- json_merge (python-dap/src/dap/json/json_utils.py)
- json_extract_keys (python-dap/src/dap/json/json_utils.py)
- json_flatten (python-dap/src/dap/json/json_utils.py)

Classes:
- DapJSONError (python-dap/src/dap/json/json_utils.py)
- DateTimeEncoder (python-dap/src/dap/json/json_utils.py)
  - DateTimeEncoder.default (python-dap/src/dap/json/json_utils.py)
- JSON (python-dap/src/dap/json/json_utils.py)
  - JSON.__init__ (python-dap/src/dap/json/json_utils.py)
  - JSON.from_file (python-dap/src/dap/json/json_utils.py)
  - JSON.from_string (python-dap/src/dap/json/json_utils.py)
  - JSON.get (python-dap/src/dap/json/json_utils.py)
  - JSON.set (python-dap/src/dap/json/json_utils.py)
  - JSON.has (python-dap/src/dap/json/json_utils.py)
  - JSON.delete (python-dap/src/dap/json/json_utils.py)
  - JSON.update (python-dap/src/dap/json/json_utils.py)
  - JSON.merge (python-dap/src/dap/json/json_utils.py)
  - JSON.extract (python-dap/src/dap/json/json_utils.py)
  - JSON.flatten (python-dap/src/dap/json/json_utils.py)
  - JSON.validate (python-dap/src/dap/json/json_utils.py)
  - JSON.to_string (python-dap/src/dap/json/json_utils.py)
  - JSON.to_dict (python-dap/src/dap/json/json_utils.py)
  - JSON.save (python-dap/src/dap/json/json_utils.py)
  - JSON.pretty (python-dap/src/dap/json/json_utils.py)
  - JSON.__getitem__ (python-dap/src/dap/json/json_utils.py)
  - JSON.__setitem__ (python-dap/src/dap/json/json_utils.py)
  - JSON.__delitem__ (python-dap/src/dap/json/json_utils.py)
  - JSON.__contains__ (python-dap/src/dap/json/json_utils.py)
  - JSON.__len__ (python-dap/src/dap/json/json_utils.py)
  - JSON.__iter__ (python-dap/src/dap/json/json_utils.py)
  - JSON.__repr__ (python-dap/src/dap/json/json_utils.py)
  - JSON.__str__ (python-dap/src/dap/json/json_utils.py)
  - JSON.__eq__ (python-dap/src/dap/json/json_utils.py)
- _Missing (python-dap/src/dap/json/json_utils.py)

### dap.log
File: python-dap/src/dap/log/__init__.py

Functions:
- set_debug_log (python-dap/src/dap/log/__init__.py)
- set_info_log (python-dap/src/dap/log/__init__.py)
- set_error_log (python-dap/src/dap/log/__init__.py)

Classes:
- DapLog (python-dap/src/dap/log/__init__.py)
  - DapLog.__init__ (python-dap/src/dap/log/__init__.py)
  - DapLog.set_log_level (python-dap/src/dap/log/__init__.py)
  - DapLog.get_log_level (python-dap/src/dap/log/__init__.py)
  - DapLog.set_level (python-dap/src/dap/log/__init__.py)
  - DapLog.get_level (python-dap/src/dap/log/__init__.py)
  - DapLog.is_level_enabled (python-dap/src/dap/log/__init__.py)
  - DapLog.list_levels (python-dap/src/dap/log/__init__.py)
  - DapLog.debug (python-dap/src/dap/log/__init__.py)
  - DapLog.info (python-dap/src/dap/log/__init__.py)
  - DapLog.error (python-dap/src/dap/log/__init__.py)
  - DapLog.warning (python-dap/src/dap/log/__init__.py)
  - DapLog.critical (python-dap/src/dap/log/__init__.py)
  - DapLog.__repr__ (python-dap/src/dap/log/__init__.py)
- DapLogError (python-dap/src/dap/log/__init__.py)
- DapLogLevel (python-dap/src/dap/log/__init__.py)
- DapLogMissingError (python-dap/src/dap/log/__init__.py)
  - DapLogMissingError.__init__ (python-dap/src/dap/log/__init__.py)

### dap.math
File: python-dap/src/dap/math/__init__.py

(No Python-level functions or classes defined)

### dap.math.uint256
File: python-dap/src/dap/math/uint256.py

Classes:
- Uint256Error (python-dap/src/dap/math/uint256.py)
- uint256 (python-dap/src/dap/math/uint256.py)
  - uint256.__init__ (python-dap/src/dap/math/uint256.py)
  - uint256.from_int (python-dap/src/dap/math/uint256.py)
  - uint256.from_decimal (python-dap/src/dap/math/uint256.py)
  - uint256.from_bytes (python-dap/src/dap/math/uint256.py)
  - uint256.zero (python-dap/src/dap/math/uint256.py)
  - uint256.to_decimal (python-dap/src/dap/math/uint256.py)
  - uint256.to_int (python-dap/src/dap/math/uint256.py)
  - uint256.to_bytes (python-dap/src/dap/math/uint256.py)
  - uint256.__str__ (python-dap/src/dap/math/uint256.py)
  - uint256.__repr__ (python-dap/src/dap/math/uint256.py)
  - uint256.__eq__ (python-dap/src/dap/math/uint256.py)
  - uint256.__lt__ (python-dap/src/dap/math/uint256.py)
  - uint256.__le__ (python-dap/src/dap/math/uint256.py)
  - uint256.__gt__ (python-dap/src/dap/math/uint256.py)
  - uint256.__ge__ (python-dap/src/dap/math/uint256.py)
  - uint256.__add__ (python-dap/src/dap/math/uint256.py)
  - uint256.__sub__ (python-dap/src/dap/math/uint256.py)
  - uint256.is_zero (python-dap/src/dap/math/uint256.py)

### dap.network
File: python-dap/src/dap/network/__init__.py

(No Python-level functions or classes defined)

### dap.network.client
File: python-dap/src/dap/network/client.py

Functions:
- create_client (python-dap/src/dap/network/client.py)
- connect_to_peer (python-dap/src/dap/network/client.py)

Classes:
- DapClient (python-dap/src/dap/network/client.py)
  - DapClient.__init__ (python-dap/src/dap/network/client.py)
  - DapClient.__del__ (python-dap/src/dap/network/client.py)
  - DapClient.connect_to (python-dap/src/dap/network/client.py)
  - DapClient.disconnect (python-dap/src/dap/network/client.py)
  - DapClient.is_connected (python-dap/src/dap/network/client.py)
  - DapClient.get_client_handle (python-dap/src/dap/network/client.py)
- DapClientError (python-dap/src/dap/network/client.py)
- DapClientManager (python-dap/src/dap/network/client.py)
  - DapClientManager.__init__ (python-dap/src/dap/network/client.py)
  - DapClientManager.create_client (python-dap/src/dap/network/client.py)
  - DapClientManager.get_client (python-dap/src/dap/network/client.py)
  - DapClientManager.remove_client (python-dap/src/dap/network/client.py)
  - DapClientManager.get_all_clients (python-dap/src/dap/network/client.py)
- DapClientStage (python-dap/src/dap/network/client.py)

### dap.network.http
File: python-dap/src/dap/network/http.py

Functions:
- create_http_client (python-dap/src/dap/network/http.py)
- quick_get (python-dap/src/dap/network/http.py)
- quick_post (python-dap/src/dap/network/http.py)
- simple_request (python-dap/src/dap/network/http.py)
- quick_http_get (python-dap/src/dap/network/http.py)
- quick_http_post (python-dap/src/dap/network/http.py)

Classes:
- DapHttp (python-dap/src/dap/network/http.py)
  - DapHttp.__init__ (python-dap/src/dap/network/http.py)
  - DapHttp._ensure_system_initialized (python-dap/src/dap/network/http.py)
  - DapHttp.create_client (python-dap/src/dap/network/http.py)
  - DapHttp.set_default_headers (python-dap/src/dap/network/http.py)
  - DapHttp.set_timeout (python-dap/src/dap/network/http.py)
  - DapHttp.request (python-dap/src/dap/network/http.py)
  - DapHttp.request_with_object (python-dap/src/dap/network/http.py)
  - DapHttp.get (python-dap/src/dap/network/http.py)
  - DapHttp.post (python-dap/src/dap/network/http.py)
  - DapHttp.put (python-dap/src/dap/network/http.py)
  - DapHttp.delete (python-dap/src/dap/network/http.py)
  - DapHttp.head (python-dap/src/dap/network/http.py)
  - DapHttp.options (python-dap/src/dap/network/http.py)
  - DapHttp.get_last_response_code (python-dap/src/dap/network/http.py)
  - DapHttp.get_last_response_size (python-dap/src/dap/network/http.py)
  - DapHttp.delete_client (python-dap/src/dap/network/http.py)
  - DapHttp.handle (python-dap/src/dap/network/http.py)
  - DapHttp.is_valid (python-dap/src/dap/network/http.py)
  - DapHttp.default_headers (python-dap/src/dap/network/http.py)
  - DapHttp.timeout (python-dap/src/dap/network/http.py)
  - DapHttp.__enter__ (python-dap/src/dap/network/http.py)
  - DapHttp.__exit__ (python-dap/src/dap/network/http.py)
  - DapHttp.__del__ (python-dap/src/dap/network/http.py)
  - DapHttp.__repr__ (python-dap/src/dap/network/http.py)
  - DapHttp._parse_url (python-dap/src/dap/network/http.py)
  - DapHttp._headers_to_string (python-dap/src/dap/network/http.py)
- DapHttpError (python-dap/src/dap/network/http.py)
- DapHttpManager (python-dap/src/dap/network/http.py)
  - DapHttpManager.get_all_clients (python-dap/src/dap/network/http.py)
  - DapHttpManager.deinitialize_system (python-dap/src/dap/network/http.py)
- DapHttpMethod (python-dap/src/dap/network/http.py)
- DapHttpNotAvailableError (python-dap/src/dap/network/http.py)
  - DapHttpNotAvailableError.__init__ (python-dap/src/dap/network/http.py)
- DapHttpRequest (python-dap/src/dap/network/http.py)
  - DapHttpRequest.__init__ (python-dap/src/dap/network/http.py)
  - DapHttpRequest.create_request (python-dap/src/dap/network/http.py)
  - DapHttpRequest.set_method (python-dap/src/dap/network/http.py)
  - DapHttpRequest.set_url (python-dap/src/dap/network/http.py)
  - DapHttpRequest.add_header (python-dap/src/dap/network/http.py)
  - DapHttpRequest.set_body (python-dap/src/dap/network/http.py)
  - DapHttpRequest.delete (python-dap/src/dap/network/http.py)
  - DapHttpRequest.handle (python-dap/src/dap/network/http.py)
  - DapHttpRequest.headers (python-dap/src/dap/network/http.py)
  - DapHttpRequest.__del__ (python-dap/src/dap/network/http.py)
- DapHttpResponse (python-dap/src/dap/network/http.py)
  - DapHttpResponse.__init__ (python-dap/src/dap/network/http.py)
  - DapHttpResponse.get_status_code (python-dap/src/dap/network/http.py)
  - DapHttpResponse.get_data (python-dap/src/dap/network/http.py)
  - DapHttpResponse.get_headers (python-dap/src/dap/network/http.py)
  - DapHttpResponse.__repr__ (python-dap/src/dap/network/http.py)

### dap.network.net
File: python-dap/src/dap/network/net.py

Functions:
- get_network (python-dap/src/dap/network/net.py)
- get_all_networks (python-dap/src/dap/network/net.py)
- create_network (python-dap/src/dap/network/net.py)

Classes:
- DapNet (python-dap/src/dap/network/net.py)
  - DapNet.__init__ (python-dap/src/dap/network/net.py)
  - DapNet.id (python-dap/src/dap/network/net.py)
  - DapNet.status (python-dap/src/dap/network/net.py)
  - DapNet.is_connected (python-dap/src/dap/network/net.py)
  - DapNet.get_chains (python-dap/src/dap/network/net.py)
  - DapNet.get_chain (python-dap/src/dap/network/net.py)
  - DapNet.add_chain (python-dap/src/dap/network/net.py)
  - DapNet.connect (python-dap/src/dap/network/net.py)
  - DapNet.disconnect (python-dap/src/dap/network/net.py)
  - DapNet.sync (python-dap/src/dap/network/net.py)
  - DapNet.get_all_networks (python-dap/src/dap/network/net.py)
  - DapNet.create_network (python-dap/src/dap/network/net.py)
  - DapNet.__str__ (python-dap/src/dap/network/net.py)
  - DapNet.__repr__ (python-dap/src/dap/network/net.py)
- DapNetID (python-dap/src/dap/network/net.py)
  - DapNetID.__init__ (python-dap/src/dap/network/net.py)
  - DapNetID.value (python-dap/src/dap/network/net.py)
  - DapNetID.name (python-dap/src/dap/network/net.py)
  - DapNetID.__str__ (python-dap/src/dap/network/net.py)
  - DapNetID.__eq__ (python-dap/src/dap/network/net.py)
- DapNetStatus (python-dap/src/dap/network/net.py)
- DapNetType (python-dap/src/dap/network/net.py)
- DapNetworkManager (python-dap/src/dap/network/net.py)
  - DapNetworkManager.__init__ (python-dap/src/dap/network/net.py)
  - DapNetworkManager.add_network (python-dap/src/dap/network/net.py)
  - DapNetworkManager.get_network (python-dap/src/dap/network/net.py)
  - DapNetworkManager.get_all_networks (python-dap/src/dap/network/net.py)
  - DapNetworkManager.set_active_network (python-dap/src/dap/network/net.py)
  - DapNetworkManager.active_network (python-dap/src/dap/network/net.py)
  - DapNetworkManager.connect_all (python-dap/src/dap/network/net.py)
  - DapNetworkManager.disconnect_all (python-dap/src/dap/network/net.py)

### dap.network.server
File: python-dap/src/dap/network/server.py

Functions:
- create_http_server (python-dap/src/dap/network/server.py)
- create_tcp_server (python-dap/src/dap/network/server.py)
- create_websocket_server (python-dap/src/dap/network/server.py)

Classes:
- DapServer (python-dap/src/dap/network/server.py)
  - DapServer.__init__ (python-dap/src/dap/network/server.py)
  - DapServer._ensure_system_initialized (python-dap/src/dap/network/server.py)
  - DapServer.create_server (python-dap/src/dap/network/server.py)
  - DapServer.create_tcp_server (python-dap/src/dap/network/server.py)
  - DapServer.add_listener (python-dap/src/dap/network/server.py)
  - DapServer.remove_listener (python-dap/src/dap/network/server.py)
  - DapServer.get_listeners (python-dap/src/dap/network/server.py)
  - DapServer.start (python-dap/src/dap/network/server.py)
  - DapServer.stop (python-dap/src/dap/network/server.py)
  - DapServer.add_processor (python-dap/src/dap/network/server.py)
  - DapServer.get_clients_count (python-dap/src/dap/network/server.py)
  - DapServer.get_clients (python-dap/src/dap/network/server.py)
  - DapServer.disconnect_client (python-dap/src/dap/network/server.py)
  - DapServer.set_callbacks (python-dap/src/dap/network/server.py)
  - DapServer.set_new_client_callback (python-dap/src/dap/network/server.py)
  - DapServer.set_client_disconnect_callback (python-dap/src/dap/network/server.py)
  - DapServer.set_auth_required (python-dap/src/dap/network/server.py)
  - DapServer.set_ssl_enabled (python-dap/src/dap/network/server.py)
  - DapServer.set_worker_count (python-dap/src/dap/network/server.py)
  - DapServer.get_worker_count (python-dap/src/dap/network/server.py)
  - DapServer.process_http_request (python-dap/src/dap/network/server.py)
  - DapServer.process_stream_request (python-dap/src/dap/network/server.py)
  - DapServer.delete (python-dap/src/dap/network/server.py)
  - DapServer.handle (python-dap/src/dap/network/server.py)
  - DapServer.name (python-dap/src/dap/network/server.py)
  - DapServer.is_valid (python-dap/src/dap/network/server.py)
  - DapServer.listeners_count (python-dap/src/dap/network/server.py)
  - DapServer.__enter__ (python-dap/src/dap/network/server.py)
  - DapServer.__exit__ (python-dap/src/dap/network/server.py)
  - DapServer.__del__ (python-dap/src/dap/network/server.py)
  - DapServer.__repr__ (python-dap/src/dap/network/server.py)
- DapServerError (python-dap/src/dap/network/server.py)
- DapServerManager (python-dap/src/dap/network/server.py)
  - DapServerManager.get_all_servers (python-dap/src/dap/network/server.py)
  - DapServerManager.deinitialize_system (python-dap/src/dap/network/server.py)
- DapServerMissingError (python-dap/src/dap/network/server.py)
  - DapServerMissingError.__init__ (python-dap/src/dap/network/server.py)

### dap.network.stream
File: python-dap/src/dap/network/stream.py

Functions:
- create_stream (python-dap/src/dap/network/stream.py)
- create_stream_worker (python-dap/src/dap/network/stream.py)
- connect_stream (python-dap/src/dap/network/stream.py)

Classes:
- DapStream (python-dap/src/dap/network/stream.py)
  - DapStream.__init__ (python-dap/src/dap/network/stream.py)
  - DapStream._ensure_system_initialized (python-dap/src/dap/network/stream.py)
  - DapStream.create_stream (python-dap/src/dap/network/stream.py)
  - DapStream.connect_to (python-dap/src/dap/network/stream.py)
  - DapStream.close (python-dap/src/dap/network/stream.py)
  - DapStream.write_data (python-dap/src/dap/network/stream.py)
  - DapStream.read_data (python-dap/src/dap/network/stream.py)
  - DapStream.get_stream_id (python-dap/src/dap/network/stream.py)
  - DapStream.get_remote_address (python-dap/src/dap/network/stream.py)
  - DapStream.get_remote_port (python-dap/src/dap/network/stream.py)
  - DapStream.set_callbacks (python-dap/src/dap/network/stream.py)
  - DapStream.set_read_callback (python-dap/src/dap/network/stream.py)
  - DapStream.set_write_callback (python-dap/src/dap/network/stream.py)
  - DapStream.set_error_callback (python-dap/src/dap/network/stream.py)
  - DapStream.create_channel (python-dap/src/dap/network/stream.py)
  - DapStream.get_channel (python-dap/src/dap/network/stream.py)
  - DapStream.delete_channel (python-dap/src/dap/network/stream.py)
  - DapStream.delete (python-dap/src/dap/network/stream.py)
  - DapStream.handle (python-dap/src/dap/network/stream.py)
  - DapStream.state (python-dap/src/dap/network/stream.py)
  - DapStream.is_valid (python-dap/src/dap/network/stream.py)
  - DapStream.channels_count (python-dap/src/dap/network/stream.py)
  - DapStream.__enter__ (python-dap/src/dap/network/stream.py)
  - DapStream.__exit__ (python-dap/src/dap/network/stream.py)
  - DapStream.__del__ (python-dap/src/dap/network/stream.py)
  - DapStream.__repr__ (python-dap/src/dap/network/stream.py)
- DapStreamChannel (python-dap/src/dap/network/stream.py)
  - DapStreamChannel.__init__ (python-dap/src/dap/network/stream.py)
  - DapStreamChannel.write (python-dap/src/dap/network/stream.py)
  - DapStreamChannel.read (python-dap/src/dap/network/stream.py)
  - DapStreamChannel.set_ready_to_read (python-dap/src/dap/network/stream.py)
  - DapStreamChannel.set_ready_to_write (python-dap/src/dap/network/stream.py)
  - DapStreamChannel.id (python-dap/src/dap/network/stream.py)
  - DapStreamChannel.handle (python-dap/src/dap/network/stream.py)
- DapStreamError (python-dap/src/dap/network/stream.py)
- DapStreamManager (python-dap/src/dap/network/stream.py)
  - DapStreamManager.get_all_streams (python-dap/src/dap/network/stream.py)
  - DapStreamManager.initialize_control (python-dap/src/dap/network/stream.py)
  - DapStreamManager.deinitialize_control (python-dap/src/dap/network/stream.py)
  - DapStreamManager.deinitialize_system (python-dap/src/dap/network/stream.py)
- DapStreamNotAvailableError (python-dap/src/dap/network/stream.py)
  - DapStreamNotAvailableError.__init__ (python-dap/src/dap/network/stream.py)
- DapStreamState (python-dap/src/dap/network/stream.py)
- DapStreamWorker (python-dap/src/dap/network/stream.py)
  - DapStreamWorker.__init__ (python-dap/src/dap/network/stream.py)
  - DapStreamWorker.create_worker (python-dap/src/dap/network/stream.py)
  - DapStreamWorker.add_stream (python-dap/src/dap/network/stream.py)
  - DapStreamWorker.remove_stream (python-dap/src/dap/network/stream.py)
  - DapStreamWorker.get_stream_count (python-dap/src/dap/network/stream.py)
  - DapStreamWorker.get_stats (python-dap/src/dap/network/stream.py)
  - DapStreamWorker.delete (python-dap/src/dap/network/stream.py)
  - DapStreamWorker.handle (python-dap/src/dap/network/stream.py)
  - DapStreamWorker.name (python-dap/src/dap/network/stream.py)

### dap.system
File: python-dap/src/dap/system/__init__.py

Functions:
- exec (python-dap/src/dap/system/__init__.py)

Classes:
- DapSystem (python-dap/src/dap/system/__init__.py)
  - DapSystem.__init__ (python-dap/src/dap/system/__init__.py)
  - DapSystem.exec (python-dap/src/dap/system/__init__.py)
  - DapSystem.validate_command (python-dap/src/dap/system/__init__.py)
  - DapSystem.__repr__ (python-dap/src/dap/system/__init__.py)
- DapSystemError (python-dap/src/dap/system/__init__.py)

### dap.time
File: python-dap/src/dap/time/__init__.py

(No Python-level functions or classes defined)

### dap.time.time
File: python-dap/src/dap/time/time.py

Functions:
- now (python-dap/src/dap/time/time.py)
- to_rfc822 (python-dap/src/dap/time/time.py)
- format_duration (python-dap/src/dap/time/time.py)

Classes:
- DapTime (python-dap/src/dap/time/time.py)
  - DapTime.__init__ (python-dap/src/dap/time/time.py)
  - DapTime.now (python-dap/src/dap/time/time.py)
  - DapTime.to_rfc822 (python-dap/src/dap/time/time.py)
  - DapTime.from_python (python-dap/src/dap/time/time.py)
  - DapTime.to_python (python-dap/src/dap/time/time.py)
  - DapTime.to_str_rfc822 (python-dap/src/dap/time/time.py)
  - DapTime.now_sec (python-dap/src/dap/time/time.py)
  - DapTime.now_usec (python-dap/src/dap/time/time.py)
  - DapTime.from_str_rfc822 (python-dap/src/dap/time/time.py)
  - DapTime.__repr__ (python-dap/src/dap/time/time.py)
- DapTimeError (python-dap/src/dap/time/time.py)

### dap.time.timer
File: python-dap/src/dap/time/timer.py

Classes:
- DapTimer (python-dap/src/dap/time/timer.py)
  - DapTimer.__init__ (python-dap/src/dap/time/timer.py)
  - DapTimer.start (python-dap/src/dap/time/timer.py)
  - DapTimer.start_on_worker (python-dap/src/dap/time/timer.py)
  - DapTimer.reset (python-dap/src/dap/time/timer.py)
  - DapTimer.delete (python-dap/src/dap/time/timer.py)
  - DapTimer.__repr__ (python-dap/src/dap/time/timer.py)
- DapTimerError (python-dap/src/dap/time/timer.py)

## C extension methods (python_dap module)

Method names are extracted from PyMethodDef arrays in C sources.

### python_dap.c
File: python-dap/src/python_dap.c

- dap_sdk_init (python-dap/src/python_dap.c)
- dap_sdk_deinit (python-dap/src/python_dap.c)

### python_dap_client.c
File: python-dap/src/python_dap_client.c

- dap_client_new (python-dap/src/python_dap_client.c)
- dap_client_delete (python-dap/src/python_dap_client.c)
- dap_client_connect_to (python-dap/src/python_dap_client.c)
- dap_client_disconnect (python-dap/src/python_dap_client.c)
- dap_client_go_stage (python-dap/src/python_dap_client.c)
- dap_client_get_stage (python-dap/src/python_dap_client.c)

### python_dap_common.c
File: python-dap/src/python_dap_common.c

- dap_common_init (python-dap/src/python_dap_common.c)
- dap_common_deinit (python-dap/src/python_dap_common.c)

### python_dap_config.c
File: python-dap/src/python_dap_config.c

- py_dap_config_init (python-dap/src/python_dap_config.c)
- dap_config_init (python-dap/src/python_dap_config.c)
- py_dap_config_deinit (python-dap/src/python_dap_config.c)
- dap_config_deinit (python-dap/src/python_dap_config.c)
- py_dap_config_open (python-dap/src/python_dap_config.c)
- py_dap_config_close (python-dap/src/python_dap_config.c)
- py_dap_config_get_item_str (python-dap/src/python_dap_config.c)
- py_dap_config_get_item_int (python-dap/src/python_dap_config.c)
- py_dap_config_get_item_uint (python-dap/src/python_dap_config.c)
- py_dap_config_get_item_bool (python-dap/src/python_dap_config.c)
- py_dap_config_get_item_double (python-dap/src/python_dap_config.c)
- py_dap_config_get_item_path (python-dap/src/python_dap_config.c)
- py_dap_config_set_item_str (python-dap/src/python_dap_config.c)
- py_dap_config_set_item_int (python-dap/src/python_dap_config.c)
- py_dap_config_set_item_bool (python-dap/src/python_dap_config.c)
- py_dap_config_get_sys_dir (python-dap/src/python_dap_config.c)

### python_dap_context.c
File: python-dap/src/python_dap_context.c

- dap_context_current (python-dap/src/python_dap_context.c)

### python_dap_cpu_monitor.c
File: python-dap/src/python_dap_cpu_monitor.c

- dap_cpu_monitor_init (python-dap/src/python_dap_cpu_monitor.c)
- dap_cpu_monitor_deinit (python-dap/src/python_dap_cpu_monitor.c)
- dap_cpu_get_stats (python-dap/src/python_dap_cpu_monitor.c)

### python_dap_crypto.c
File: python-dap/src/python_dap_crypto.c

- py_dap_crypto_key_create (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_key_create_from_seed (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_key_delete (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_key_get_public (python-dap/src/python_dap_crypto.c)
- py_dap_sign_delete (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_key_sign (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_key_verify (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_key_encrypt (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_key_decrypt (python-dap/src/python_dap_crypto.c)
- py_dap_hash_fast_create (python-dap/src/python_dap_crypto.c)
- py_dap_hash_fast_delete (python-dap/src/python_dap_crypto.c)
- py_dap_hash_fast (python-dap/src/python_dap_crypto.c)
- py_dap_cert_create (python-dap/src/python_dap_crypto.c)
- py_dap_cert_create_from_key (python-dap/src/python_dap_crypto.c)
- py_dap_cert_generate (python-dap/src/python_dap_crypto.c)
- py_dap_cert_add (python-dap/src/python_dap_crypto.c)
- py_dap_cert_add_folder (python-dap/src/python_dap_crypto.c)
- py_dap_cert_delete (python-dap/src/python_dap_crypto.c)
- py_dap_cert_sign (python-dap/src/python_dap_crypto.c)
- py_dap_cert_save_to_file (python-dap/src/python_dap_crypto.c)
- py_dap_cert_load_from_file (python-dap/src/python_dap_crypto.c)
- py_dap_cert_save (python-dap/src/python_dap_crypto.c)
- py_dap_cert_load (python-dap/src/python_dap_crypto.c)
- py_dap_cert_load_from_data (python-dap/src/python_dap_crypto.c)
- py_dap_cert_verify (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_multi_sign_create (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_multi_sign_add (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_multi_sign_combine (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_multi_sign_verify (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_aggregated_sign_add (python-dap/src/python_dap_crypto.c)
- py_dap_crypto_multi_sign_delete (python-dap/src/python_dap_crypto.c)
- py_dap_multi_sign_params_make (python-dap/src/python_dap_crypto.c)
- py_dap_multi_sign_params_delete (python-dap/src/python_dap_crypto.c)
- py_dap_multi_sign_key_new (python-dap/src/python_dap_crypto.c)
- py_dap_sign_create (python-dap/src/python_dap_crypto.c)
- py_dap_sign_create_with_hash_type (python-dap/src/python_dap_crypto.c)
- py_dap_sign_verify (python-dap/src/python_dap_crypto.c)
- py_dap_sign_verify_by_pkey (python-dap/src/python_dap_crypto.c)
- py_dap_sign_verify_all (python-dap/src/python_dap_crypto.c)
- py_dap_sign_get_size (python-dap/src/python_dap_crypto.c)
- py_dap_sign_get_bytes (python-dap/src/python_dap_crypto.c)
- py_dap_sign_verify_size (python-dap/src/python_dap_crypto.c)
- py_dap_sign_type_from_key_type (python-dap/src/python_dap_crypto.c)
- py_dap_sign_type_to_key_type (python-dap/src/python_dap_crypto.c)
- py_dap_sign_get_str_recommended_types (python-dap/src/python_dap_crypto.c)
- py_dap_sign_aggregate_signatures (python-dap/src/python_dap_crypto.c)
- py_dap_sign_verify_aggregated (python-dap/src/python_dap_crypto.c)
- py_dap_sign_type_supports_aggregation (python-dap/src/python_dap_crypto.c)
- py_dap_sign_is_aggregated (python-dap/src/python_dap_crypto.c)

### python_dap_events.c
File: python-dap/src/python_dap_events.c

- dap_events_init (python-dap/src/python_dap_events.c)
- dap_events_deinit (python-dap/src/python_dap_events.c)
- dap_events_start (python-dap/src/python_dap_events.c)
- dap_events_stop (python-dap/src/python_dap_events.c)
- dap_events_socket_create (python-dap/src/python_dap_events.c)
- dap_events_socket_delete (python-dap/src/python_dap_events.c)
- dap_events_socket_queue_ptr (python-dap/src/python_dap_events.c)
- dap_events_socket_assign_on_worker_mt (python-dap/src/python_dap_events.c)
- dap_events_socket_event_proc_add (python-dap/src/python_dap_events.c)
- dap_events_socket_event_proc_remove (python-dap/src/python_dap_events.c)

### python_dap_http.c
File: python-dap/src/python_dap_http.c

- dap_http_client_init (python-dap/src/python_dap_http.c)
- dap_http_client_deinit (python-dap/src/python_dap_http.c)
- dap_http_client_new (python-dap/src/python_dap_http.c)
- dap_http_client_delete (python-dap/src/python_dap_http.c)
- dap_http_client_request (python-dap/src/python_dap_http.c)
- dap_http_client_request_ex (python-dap/src/python_dap_http.c)
- dap_http_client_set_timeout (python-dap/src/python_dap_http.c)
- dap_http_client_get_response_code (python-dap/src/python_dap_http.c)
- dap_http_client_get_response_size (python-dap/src/python_dap_http.c)
- dap_http_client_get_response_data (python-dap/src/python_dap_http.c)
- dap_http_client_get_response_headers (python-dap/src/python_dap_http.c)
- dap_http_request_new (python-dap/src/python_dap_http.c)
- dap_http_request_delete (python-dap/src/python_dap_http.c)
- dap_http_request_add_header (python-dap/src/python_dap_http.c)
- dap_http_request_set_body (python-dap/src/python_dap_http.c)
- dap_http_request_set_method (python-dap/src/python_dap_http.c)
- dap_http_request_set_url (python-dap/src/python_dap_http.c)
- dap_http_response_new (python-dap/src/python_dap_http.c)
- dap_http_response_delete (python-dap/src/python_dap_http.c)
- dap_http_response_get_code (python-dap/src/python_dap_http.c)
- dap_http_response_get_data (python-dap/src/python_dap_http.c)
- dap_http_response_get_headers (python-dap/src/python_dap_http.c)
- dap_http_response_get_header (python-dap/src/python_dap_http.c)
- dap_http_get_all_clients (python-dap/src/python_dap_http.c)

### python_dap_json_rpc.c
File: python-dap/src/python_dap_json_rpc.c

- json_rpc_init (python-dap/src/python_dap_json_rpc.c)
- json_rpc_deinit (python-dap/src/python_dap_json_rpc.c)
- json_rpc_process_request (python-dap/src/python_dap_json_rpc.c)
- json_rpc_register_method_handler (python-dap/src/python_dap_json_rpc.c)
- json_rpc_unregister_method_handler (python-dap/src/python_dap_json_rpc.c)
- json_rpc_register_url_handler (python-dap/src/python_dap_json_rpc.c)
- json_rpc_unregister_url_handler (python-dap/src/python_dap_json_rpc.c)
- json_rpc_request_create (python-dap/src/python_dap_json_rpc.c)
- json_rpc_request_to_json (python-dap/src/python_dap_json_rpc.c)
- json_rpc_request_free (python-dap/src/python_dap_json_rpc.c)
- json_rpc_response_from_string (python-dap/src/python_dap_json_rpc.c)
- json_rpc_response_to_string (python-dap/src/python_dap_json_rpc.c)
- json_rpc_response_free (python-dap/src/python_dap_json_rpc.c)
- json_rpc_exec_cmd_inited (python-dap/src/python_dap_json_rpc.c)

### python_dap_logging.c
File: python-dap/src/python_dap_logging.c

- dap_set_log_level (python-dap/src/python_dap_logging.c)
- dap_log_level_set (python-dap/src/python_dap_logging.c)
- dap_get_log_level (python-dap/src/python_dap_logging.c)
- dap_log_set_external_output (python-dap/src/python_dap_logging.c)
- py_dap_log_it_debug (python-dap/src/python_dap_logging.c)
- py_dap_log_it_info (python-dap/src/python_dap_logging.c)
- py_dap_log_it_warning (python-dap/src/python_dap_logging.c)
- py_dap_log_it_error (python-dap/src/python_dap_logging.c)

### python_dap_network.c
File: python-dap/src/python_dap_network.c

- dap_network_init (python-dap/src/python_dap_network.c)
- dap_network_deinit (python-dap/src/python_dap_network.c)
- dap_network_get_status (python-dap/src/python_dap_network.c)
- dap_network_reinit_stream (python-dap/src/python_dap_network.c)
- dap_network_reinit_http (python-dap/src/python_dap_network.c)
- dap_network_reinit_client (python-dap/src/python_dap_network.c)

### python_dap_plugin.c
File: python-dap/src/python_dap_plugin.c

- plugin_init (python-dap/src/python_dap_plugin.c)
- plugin_deinit (python-dap/src/python_dap_plugin.c)
- plugin_type_create (python-dap/src/python_dap_plugin.c)
- plugin_start_all (python-dap/src/python_dap_plugin.c)
- plugin_stop_all (python-dap/src/python_dap_plugin.c)
- plugin_status (python-dap/src/python_dap_plugin.c)
- plugin_start (python-dap/src/python_dap_plugin.c)
- plugin_stop (python-dap/src/python_dap_plugin.c)

### python_dap_process_manager.c
File: python-dap/src/python_dap_process_manager.c

- save_process_pid_in_file (python-dap/src/python_dap_process_manager.c)
- get_pid_from_file (python-dap/src/python_dap_process_manager.c)
- is_process_running (python-dap/src/python_dap_process_manager.c)
- daemonize_process (python-dap/src/python_dap_process_manager.c)
- kill_process (python-dap/src/python_dap_process_manager.c)

### python_dap_server.c
File: python-dap/src/python_dap_server.c

- server_new (python-dap/src/python_dap_server.c)
- server_delete (python-dap/src/python_dap_server.c)
- server_listen (python-dap/src/python_dap_server.c)
- server_start (python-dap/src/python_dap_server.c)
- server_stop (python-dap/src/python_dap_server.c)
- server_init (python-dap/src/python_dap_server.c)
- server_deinit (python-dap/src/python_dap_server.c)
- server_get_all (python-dap/src/python_dap_server.c)
- dap_server_new (python-dap/src/python_dap_server.c)
- dap_server_delete (python-dap/src/python_dap_server.c)
- dap_server_listen (python-dap/src/python_dap_server.c)
- dap_server_start (python-dap/src/python_dap_server.c)
- dap_server_stop (python-dap/src/python_dap_server.c)

### python_dap_stream.c
File: python-dap/src/python_dap_stream.c

- dap_stream_init (python-dap/src/python_dap_stream.c)
- dap_stream_deinit (python-dap/src/python_dap_stream.c)
- dap_stream_new (python-dap/src/python_dap_stream.c)
- dap_stream_delete (python-dap/src/python_dap_stream.c)
- dap_stream_open (python-dap/src/python_dap_stream.c)
- dap_stream_close (python-dap/src/python_dap_stream.c)
- dap_stream_write (python-dap/src/python_dap_stream.c)
- dap_stream_read (python-dap/src/python_dap_stream.c)
- dap_stream_get_id (python-dap/src/python_dap_stream.c)
- dap_stream_set_callbacks (python-dap/src/python_dap_stream.c)
- dap_stream_get_remote_addr (python-dap/src/python_dap_stream.c)
- dap_stream_get_remote_port (python-dap/src/python_dap_stream.c)
- dap_stream_ch_new (python-dap/src/python_dap_stream.c)
- dap_stream_ch_delete (python-dap/src/python_dap_stream.c)
- dap_stream_ch_write (python-dap/src/python_dap_stream.c)
- dap_stream_ch_read (python-dap/src/python_dap_stream.c)
- dap_stream_ch_set_ready_to_read (python-dap/src/python_dap_stream.c)
- dap_stream_ch_set_ready_to_write (python-dap/src/python_dap_stream.c)
- dap_stream_ch_pkt_write (python-dap/src/python_dap_stream.c)
- dap_stream_ch_pkt_send (python-dap/src/python_dap_stream.c)
- dap_stream_ch_add_notifier (python-dap/src/python_dap_stream.c)
- dap_stream_ch_del_notifier (python-dap/src/python_dap_stream.c)
- dap_stream_worker_new (python-dap/src/python_dap_stream.c)
- dap_stream_worker_delete (python-dap/src/python_dap_stream.c)
- dap_stream_worker_add_stream (python-dap/src/python_dap_stream.c)
- dap_stream_worker_remove_stream (python-dap/src/python_dap_stream.c)
- dap_stream_worker_get_count (python-dap/src/python_dap_stream.c)
- dap_stream_worker_get_stats (python-dap/src/python_dap_stream.c)
- dap_stream_ctl_init_py (python-dap/src/python_dap_stream.c)
- dap_stream_ctl_deinit (python-dap/src/python_dap_stream.c)
- dap_stream_get_all (python-dap/src/python_dap_stream.c)
- dap_stream_set_node_addr_from_cert (python-dap/src/python_dap_stream.c)

### python_dap_system.c
File: python-dap/src/python_dap_system.c

- py_dap_malloc (python-dap/src/python_dap_system.c)
- py_dap_free (python-dap/src/python_dap_system.c)
- py_dap_calloc (python-dap/src/python_dap_system.c)
- py_dap_realloc (python-dap/src/python_dap_system.c)
- py_exec_with_ret_multistring (python-dap/src/python_dap_system.c)
- dap_malloc (python-dap/src/python_dap_system.c)
- dap_free (python-dap/src/python_dap_system.c)
- dap_calloc (python-dap/src/python_dap_system.c)
- dap_realloc (python-dap/src/python_dap_system.c)
- exec_with_ret_multistring (python-dap/src/python_dap_system.c)

### python_dap_time.c
File: python-dap/src/python_dap_time.c

- py_dap_time_now (python-dap/src/python_dap_time.c)
- dap_time_now (python-dap/src/python_dap_time.c)
- dap_time_to_str_rfc822 (python-dap/src/python_dap_time.c)
- dap_time_to_str_rfc3339 (python-dap/src/python_dap_time.c)
- dap_time_from_str_rfc822 (python-dap/src/python_dap_time.c)
- dap_usleep (python-dap/src/python_dap_time.c)
- dap_sleep (python-dap/src/python_dap_time.c)

### python_dap_uint256.c
File: python-dap/src/python_dap_uint256.c

- uint256_from_uint64 (python-dap/src/python_dap_uint256.c)
- uint256_from_decimal (python-dap/src/python_dap_uint256.c)
- uint256_from_bytes (python-dap/src/python_dap_uint256.c)
- uint256_to_decimal (python-dap/src/python_dap_uint256.c)
- uint256_to_uint64 (python-dap/src/python_dap_uint256.c)
- uint256_to_bytes (python-dap/src/python_dap_uint256.c)
- uint256_is_zero (python-dap/src/python_dap_uint256.c)
- uint256_compare (python-dap/src/python_dap_uint256.c)
- uint256_equal (python-dap/src/python_dap_uint256.c)
- uint256_add (python-dap/src/python_dap_uint256.c)
- uint256_sub (python-dap/src/python_dap_uint256.c)
- uint256_mul (python-dap/src/python_dap_uint256.c)
- uint256_div (python-dap/src/python_dap_uint256.c)
- uint256_mod (python-dap/src/python_dap_uint256.c)

### python_dap_worker.c
File: python-dap/src/python_dap_worker.c

- dap_events_worker_get_auto (python-dap/src/python_dap_worker.c)
- dap_worker_get_index (python-dap/src/python_dap_worker.c)
- dap_worker_add_events_socket (python-dap/src/python_dap_worker.c)

### python_global_db.c
File: python-dap/src/python_global_db.c

- dap_global_db_init (python-dap/src/python_global_db.c)
- dap_global_db_deinit (python-dap/src/python_global_db.c)
- dap_global_db_set (python-dap/src/python_global_db.c)
- dap_global_db_get (python-dap/src/python_global_db.c)
- dap_global_db_del (python-dap/src/python_global_db.c)
- dap_global_db_get_keys (python-dap/src/python_global_db.c)
- dap_global_db_set_signing_cert (python-dap/src/python_global_db.c)
- dap_global_db_cluster_new (python-dap/src/python_global_db.c)
- dap_global_db_cluster_delete (python-dap/src/python_global_db.c)
- dap_global_db_cluster_member_add (python-dap/src/python_global_db.c)
- dap_global_db_cluster_member_delete (python-dap/src/python_global_db.c)
- dap_global_db_cluster_notify (python-dap/src/python_global_db.c)
- dap_global_db_cluster_add_notify_callback (python-dap/src/python_global_db.c)
- dap_global_db_cluster_broadcast (python-dap/src/python_global_db.c)
- dap_global_db_cluster_exists (python-dap/src/python_global_db.c)

