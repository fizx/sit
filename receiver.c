#include "sit.h"

Receiver *
sit_receiver_new() {
  return calloc(1, sizeof(Receiver));
}
