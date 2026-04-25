#include "include/MprpcController.h"

MprpcController::MprpcController() {
    _failed = false;
    _errot_text = "";
}

void MprpcController::Reset() {
    _failed = false;
    _errot_text = "";
}

bool MprpcController::Failed() const {
    return _failed;
}

std::string MprpcController::ErrorText() const {
    return _errot_text;
}

void MprpcController::StartCancel() { }

void MprpcController::SetFailed(std::string const &reason) {
    _failed = true;
    _errot_text = reason;
}

bool MprpcController::IsCanceled() const {
    return false;
}

void MprpcController::NotifyOnCancel(::google::protobuf::Closure *callback) { }
