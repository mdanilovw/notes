#include "core_action.hpp"
#include "return_code.hpp"

void CoreAction::setCore(const shared_ptr<Core> &core) {
    this->core = core;
}

future<Response> CoreAction::getFuture() {
    return responsePromise.get_future();
}

void AddRecordAction::exec() {
    auto code = core->addRecord({ std::move(text), std::move(tags) });
    responsePromise.set_value({ code });
}

void AddRecordAction::undo() {
    throw "Undo not implemented for AddRecord";
}

void UpdateRecordAction::exec() {
    auto code = core->updateRecord(std::move(record));
    responsePromise.set_value({ code });
}

void UpdateRecordAction::undo() {
    throw "Undo not implemented for Update Record";
}

void SearchRecordsAction::exec() {
    auto records = core->search(pred);

    if(!records.empty()) {
        responsePromise.set_value({ ReturnCode::OK, std::move(records) });
    } else {
        responsePromise.set_value({ ReturnCode::NOT_FOUND });
    }
}

void SearchRecordsAction::undo() {
    // Does nothing
}

void SetPasswordAction::exec() {
    auto code = core->setPassword(std::move(password));
    responsePromise.set_value({ code });
}

void SetPasswordAction::undo() {
    throw "Undo not implemented for Set Password action";
}

void StartAction::exec() {
    auto code = core->start();
    responsePromise.set_value({ code });
}

void StartAction::undo() {
    throw "Undo not implemented for Start action";
}